#include "x86/i386/memory/paging.hpp"

#include <cstdarg>
#include <cstring>

#include <stdio.h>

#include "hal/paging.hpp"

namespace i386::mem {

static constexpr uint32_t PdMask = 0xFFFFF000u;
static constexpr uint32_t OffMask = 0xFFFu;

static inline uint32_t read_cr3() noexcept {
  uint32_t v;
  asm volatile("mov %%cr3, %0" : "=r"(v));
  return v;
}

static inline void write_cr3(uint32_t v) noexcept {
  asm volatile("mov %0, %%cr3" ::"r"(v) : "memory");
}

uint32_t Paging::hw_flags(hal::PageFlags f) noexcept {
  uint32_t e = 0;
  if ((f & hal::PageFlags::Writable) == hal::PageFlags::Writable) e |= (1u << 1);
  if ((f & hal::PageFlags::User) == hal::PageFlags::User) e |= (1u << 2);
  if ((f & hal::PageFlags::WriteThrough) == hal::PageFlags::WriteThrough) e |= (1u << 3);
  if ((f & hal::PageFlags::CacheDisable) == hal::PageFlags::CacheDisable) e |= (1u << 4);
  if ((f & hal::PageFlags::Global) == hal::PageFlags::Global) e |= (1u << 8);
  e |= (1u << 0);
  return e;
}

hal::PageFlags Paging::from_hw(uint32_t e) noexcept {
  hal::PageFlags f = hal::PageFlags::None;
  if (e & (1u << 0)) f |= hal::PageFlags::Present;
  if (e & (1u << 1)) f |= hal::PageFlags::Writable;
  if (e & (1u << 2)) f |= hal::PageFlags::User;
  if (e & (1u << 3)) f |= hal::PageFlags::WriteThrough;
  if (e & (1u << 4)) f |= hal::PageFlags::CacheDisable;
  if (e & (1u << 8)) f |= hal::PageFlags::Global;
  return f;
}

uint32_t* Paging::pd_virt_current() const noexcept {
  return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(read_cr3() & PdMask));
}

void* Paging::k_map_frame(uintptr_t phys) noexcept {
  auto* pd = pd_virt_current();
  uint32_t pde = pd[1023];
  auto* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & 0xFFFFF000u));

  uint32_t ti = (KMapSlot >> 12) & 0x3FFu;
  pt[ti] = (static_cast<uint32_t>(phys) & 0xFFFFF000u) | 0x3u;

  flush(KMapSlot);
  return reinterpret_cast<void*>(KMapSlot);
}

void Paging::k_unmap_frame() noexcept {
  auto* pd = pd_virt_current();
  uint32_t pde = pd[1023];
  auto* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & 0xFFFFF000u));

  uint32_t ti = (KMapSlot >> 12) & 0x3FFu;
  pt[ti] = 0;

  flush(KMapSlot);
}

uint32_t* Paging::get_pt(uint32_t* pd, uint32_t pdi) const noexcept {
  uint32_t pde = pd[pdi];
  if ((pde & 1u) == 0) return nullptr;
  return reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & PdMask));
}

bool Paging::ensure_pt(uint32_t* pd, uint32_t pdi, uint32_t pde_flags) noexcept {
  if (pd[pdi] & 1u) return true;

  uintptr_t pt_phys = pfa.alloc_frame();
  if (!pt_phys) return false;

  void* pt = k_map_frame(pt_phys);
  memset(pt, 0, PageSize);
  k_unmap_frame();

  pd[pdi] = static_cast<uint32_t>((pt_phys & PdMask) | (pde_flags & 0xFFFu) | 0x1u);
  return true;
}

bool Paging::init_kernel_space() noexcept {
  uintptr_t new_pd_phys = pfa.alloc_frame();
  if (!new_pd_phys) return false;

  auto* new_pd = reinterpret_cast<uint32_t*>(new_pd_phys);
  memset(new_pd, 0, PageSize);

  auto* cur_pd = pd_virt_current();
  memcpy(new_pd, cur_pd, PageSize);

  kernel_as.pd_phys = new_pd_phys;
  switch_to(kernel_as);
  return true;
}

void Paging::switch_to(const AddressSpace& as) noexcept {
  write_cr3(static_cast<uint32_t>(as.pd_phys));
}

bool Paging::create_user_space(AddressSpace& out) noexcept {
  uintptr_t pd_phys = pfa.alloc_frame();
  if (!pd_phys) return false;

  void* new_pd_v = k_map_frame(pd_phys);
  memset(new_pd_v, 0, PageSize);

  uint32_t* new_pd = reinterpret_cast<uint32_t*>(new_pd_v);
  uint32_t* kernel_pd =
      reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(kernel_as.pd_phys));

  for (uint32_t i = KernelPdeBase; i < 1024; ++i) {
    new_pd[i] = kernel_pd[i];
  }

  k_unmap_frame();

  out.pd_phys = pd_phys;
  return true;
}

void Paging::destroy_space(const AddressSpace& as) noexcept {
  if (!as.pd_phys || as.pd_phys == kernel_as.pd_phys) return;

  void* pd_v = k_map_frame(as.pd_phys);
  uint32_t* pd = reinterpret_cast<uint32_t*>(pd_v);

  for (uint32_t i = 0; i < KernelPdeBase; ++i) {
    uint32_t pde = pd[i];
    if ((pde & 1u) == 0) continue;
    uintptr_t pt_phys = static_cast<uintptr_t>(pde & PdMask);
    pfa.free_frame(pt_phys);
    pd[i] = 0;
  }

  k_unmap_frame();
  pfa.free_frame(as.pd_phys);
}

bool Paging::map(uintptr_t vaddr, uintptr_t paddr, hal::PageFlags flags) noexcept {
  vaddr &= ~static_cast<uintptr_t>(PageSize - 1);
  paddr &= ~static_cast<uintptr_t>(PageSize - 1);

  uint32_t* pd = pd_virt_current();
  uint32_t di = pdi(vaddr);
  uint32_t ti = pti(vaddr);

  bool is_user = vaddr < KernelBase;
  uint32_t pde_flags = (1u << 1);
  if (is_user) pde_flags |= (1u << 2);

  if (!ensure_pt(pd, di, pde_flags)) return false;

  uint32_t* pt = get_pt(pd, di);
  if (!pt) return false;

  uint32_t entry = static_cast<uint32_t>((paddr & PdMask) | hw_flags(flags));
  pt[ti] = entry;
  return true;
}

bool Paging::map_range(uintptr_t vaddr, uintptr_t paddr, size_t pages,
                       hal::PageFlags flags) noexcept {
  for (size_t i = 0; i < pages; ++i) {
    if (!map(vaddr + i * PageSize, paddr + i * PageSize, flags)) return false;
  }
  return true;
}

void Paging::unmap(uintptr_t vaddr) noexcept {
  vaddr &= ~static_cast<uintptr_t>(PageSize - 1);

  uint32_t* pd = pd_virt_current();
  uint32_t di = pdi(vaddr);
  uint32_t ti = pti(vaddr);

  uint32_t pde = pd[di];
  if ((pde & 1u) == 0) return;

  uint32_t* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & PdMask));
  pt[ti] = 0;
}

void Paging::unmap_range(uintptr_t vaddr, size_t pages) noexcept {
  for (size_t i = 0; i < pages; ++i)
    unmap(vaddr + i * PageSize);
}

bool Paging::translate(uintptr_t vaddr, uintptr_t& out_paddr,
                       hal::PageFlags& out_flags) const noexcept {
  uint32_t* pd = pd_virt_current();
  uint32_t di = pdi(vaddr);
  uint32_t ti = pti(vaddr);
  uint32_t off = static_cast<uint32_t>(vaddr & OffMask);

  uint32_t pde = pd[di];
  if ((pde & 1u) == 0) return false;

  uint32_t* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & PdMask));
  uint32_t pte = pt[ti];
  if ((pte & 1u) == 0) return false;

  out_paddr = static_cast<uintptr_t>((pte & PdMask) | off);
  out_flags = from_hw(pte);
  return true;
}

void Paging::flush(uintptr_t vaddr) noexcept {
  asm volatile("invlpg (%0)" ::"r"(vaddr) : "memory");
}

void Paging::flush_all() noexcept {
  uint32_t cr3 = read_cr3();
  write_cr3(cr3);
}

}  // namespace i386::mem
