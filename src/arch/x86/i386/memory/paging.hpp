#pragma once

#include "hal/page_frame_allocator.hpp"
#include "hal/paging.hpp"

namespace i386::mem {

struct AddressSpace {
  uintptr_t pd_phys{0};
};

class Paging final : public hal::Paging {
 public:
  static constexpr uintptr_t PageSize = 4096;
  static constexpr uintptr_t KernelBase = 0xC0000000u;
  static constexpr uint32_t KernelPdeBase = 768;

  static constexpr uintptr_t KMapSlot = 0xFFC00000u;

  Paging(hal::PageFrameAllocator& pfa) noexcept : pfa(pfa) {}

  bool init_kernel_space() noexcept;
  void switch_to(const AddressSpace& as) noexcept;

  bool create_user_space(AddressSpace& out) noexcept;
  void destroy_space(const AddressSpace& as) noexcept;

  size_t page_size() const noexcept override { return PageSize; }

  bool map(uintptr_t vaddr, uintptr_t paddr, hal::PageFlags flags) noexcept override;
  bool map_range(uintptr_t vaddr, uintptr_t paddr, size_t pages,
                 hal::PageFlags flags) noexcept override;

  void unmap(uintptr_t vaddr) noexcept override;
  void unmap_range(uintptr_t vaddr, size_t pages) noexcept override;

  bool translate(uintptr_t vaddr, uintptr_t& out_paddr,
                 hal::PageFlags& out_flags) const noexcept override;

  void flush(uintptr_t vaddr) noexcept override;
  void flush_all() noexcept override;

  const AddressSpace& kernel_space() const noexcept { return kernel_as; }

 private:
  uint32_t* pd_virt_current() const noexcept;

  static uint32_t pdi(uintptr_t v) noexcept {
    return static_cast<uint32_t>((v >> 22) & 0x3FFu);
  }

  static uint32_t pti(uintptr_t v) noexcept {
    return static_cast<uint32_t>((v >> 12) & 0x3FFu);
  }

  static uint32_t hw_flags(hal::PageFlags f) noexcept;
  static hal::PageFlags from_hw(uint32_t e) noexcept;

  void* k_map_frame(uintptr_t phys) noexcept;
  void k_unmap_frame() noexcept;

  bool ensure_pt(uint32_t* pd, uint32_t pdi, uint32_t pde_flags) noexcept;
  uint32_t* get_pt(uint32_t* pd, uint32_t pdi) const noexcept;

  hal::PageFrameAllocator& pfa;
  AddressSpace kernel_as{};
};
}  // namespace i386::mem
