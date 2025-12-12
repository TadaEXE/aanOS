#include <cstdint>
#include <cstring>
#include <iterator>

#include <kernel/log.hpp>
#include <kernel/panic.hpp>

#include "board/serial_desc.hpp"
#include "boot/boot_context.hpp"
#include "boot/multiboot2.hpp"
#include "drv/global_core.hpp"
#include "hal/boot.hpp"
#include "hal/serial.hpp"
#include "kernel.hpp"
#include "logging/backend/serial.hpp"
#include "memory/builtin/bm_heap.hpp"
#include "memory/builtin/bm_page_frame_allocator.hpp"
#include "memory/heap.hpp"
#include "x86/common/board/pc_devices.hpp"
#include "x86/common/drv/register.hpp"
#include "x86/common/graphics/framebuffer.hpp"
#include "x86/common/input/keyboard.hpp"
#include "x86/i386/memory/paging.hpp"

using namespace x86;

extern "C" uint8_t _ld_kernel_end_addr;

logging::backend::LoggingSink* setup_logging(::drv::serial::Port& serial) {
  static logging::backend::SerialSink serial_sink(serial);
  log_msg("");
  log_msg("");
  log_msg("");
  return &serial_sink;
}

namespace {

constexpr uint32_t PageSize = i386::mem::Paging::PageSize;
constexpr uint32_t KernelBase = i386::mem::Paging::KernelBase;
constexpr uint32_t KMapSlot = i386::mem::Paging::KMapSlot;

constexpr uint32_t align_up_4k(uint32_t v) noexcept {
  return (v + (PageSize - 1)) & ~(PageSize - 1);
}

constexpr uint32_t align_down_4k(uint32_t v) noexcept {
  return v & ~(PageSize - 1);
}

uint32_t mb2_end_addr(uint32_t mb2_info_addr) noexcept {
  auto* base = reinterpret_cast<const uint8_t*>(static_cast<uintptr_t>(mb2_info_addr));
  uint32_t total_size = *reinterpret_cast<const uint32_t*>(base);
  return mb2_info_addr + total_size;
}

uint32_t first_free_paddr(uint32_t mb2_info_addr) noexcept {
  uint32_t kernel_end =
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&_ld_kernel_end_addr));
  uint32_t mb2_end = mb2_end_addr(mb2_info_addr);
  uint32_t first = (kernel_end > mb2_end) ? kernel_end : mb2_end;
  return align_up_4k(first);
}

uint32_t choose_early_identity_limit(uint32_t bump) noexcept {
  uint32_t lim = bump + (16 * mem::MiB);
  if (lim < 32 * mem::MiB) lim = 32 * mem::MiB;
  return align_up_4k(lim);
}

struct EarlyPaging {
  uint32_t pd_phys{0};
  uint32_t limit{0};
};

EarlyPaging setup_early_identity_paging(uint32_t& bump, uint32_t limit) noexcept {
  constexpr uint32_t KMAP_PDE = 1023;
  EarlyPaging out{};
  out.limit = limit;

  bump = align_up_4k(bump);
  out.pd_phys = bump;
  bump += PageSize;

  auto* pd = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(out.pd_phys));
  memset(pd, 0, PageSize);

  uint32_t last_pdi = (limit == 0) ? 0 : ((limit - 1) >> 22);

  for (uint32_t di = 0; di <= last_pdi; ++di) {
    bump = align_up_4k(bump);
    uint32_t pt_phys = bump;
    bump += PageSize;

    auto* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pt_phys));
    memset(pt, 0, PageSize);
    pd[di] = (pt_phys & 0xFFFFF000u) | 0x3u;
  }

  for (uint32_t addr = 0; addr < limit; addr += PageSize) {
    uint32_t di = (addr >> 22) & 0x3FFu;
    uint32_t ti = (addr >> 12) & 0x3FFu;

    uint32_t pde = pd[di];
    auto* pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(pde & 0xFFFFF000u));
    pt[ti] = (addr & 0xFFFFF000u) | 0x3u;
  }

  bump = align_up_4k(bump);
  uint32_t kmap_pt_phys = bump;
  bump += PageSize;

  auto* kmap_pt = reinterpret_cast<uint32_t*>(static_cast<uintptr_t>(kmap_pt_phys));
  memset(kmap_pt, 0, PageSize);
  pd[KMAP_PDE] = (kmap_pt_phys & 0xFFFFF000u) | 0x3u;

  return out;
}

void enable_paging(uint32_t pd_phys) noexcept {
  asm volatile("mov %0, %%cr3" ::"r"(pd_phys) : "memory");
  uint32_t cr0;
  asm volatile("mov %%cr0, %0" : "=r"(cr0));
  cr0 |= 0x80000000u;
  asm volatile("mov %0, %%cr0" ::"r"(cr0) : "memory");
}

inline ::drv::serial::Port* find_console(::drv::GlobalCore& core) noexcept {
  using DescEntry = ::drv::GlobalCore::DescEntry;

  ::drv::serial::Port* p = core.find_if<::drv::serial::Port>(
      [](const DescEntry& de, void* iface, ::drv::DriverBase*) {
        (void)iface;
        if (de.type != ::drv::TypeTag::of<board::SerialDesc>()) { return false; }
        auto* sd = static_cast<const board::SerialDesc*>(de.ptr);
        if (!sd->name) { return false; }
        return std::strncmp(sd->name, "com1", 4) == 0;
      });

  return p;
}

void setup_boot_fb(kernel::KernelServices& kserv) {
  mb2::FramebufferTag fb_tag{};
  static graphics::Framebuffer boot_fb;

  if (mb2::load_tag<mb2::TagType::Framebuffer>(fb_tag)) {
    boot_fb = graphics::Framebuffer(fb_tag);

    if (boot_fb.valid()) { kserv.framebuffer = &boot_fb; }
  }
}

void make_mem_map(boot::BootContext& ctx) {
  static boot::MemoryRegion regions[16];
  size_t count = 0;

  mb2::MmapTag* mmap = nullptr;
  if (!mb2::load_tag<mb2::TagType::Mmap>(&mmap)) {
    ctx.memory_regions = 0;
    ctx.memory_map = nullptr;
    return;
  }

  auto* header = reinterpret_cast<mb2::TagHeader*>(reinterpret_cast<uint8_t*>(mmap) -
                                                   mb2::HeaderSize);

  auto* tag_begin = reinterpret_cast<uint8_t*>(header);
  auto* tag_end = tag_begin + header->size;

  auto* entry = mmap->entries;
  while (reinterpret_cast<uint8_t*>(entry) < tag_end) {
    using Entry = mb2::MmapTag::Entry;

    auto base = static_cast<uintptr_t>(entry->base_addr);
    auto len = static_cast<size_t>(entry->length);

    boot::MemoryRegionType type_out = boot::MemoryRegionType::Unknown;

    switch (entry->type) {
      case Entry::Type::AvailableRAM:
        type_out = boot::MemoryRegionType::Usable;
        break;
      case Entry::Type::ApciInfo:
        type_out = boot::MemoryRegionType::AcpiReclaimable;
        break;
      case Entry::Type::Reserved:
        type_out = boot::MemoryRegionType::Reserved;
        break;
      case Entry::Type::DefectiveRAM:
        type_out = boot::MemoryRegionType::Bad;
        break;
      default:
        type_out = boot::MemoryRegionType::Unknown;
        break;
    }

    if (type_out != boot::MemoryRegionType::Unknown && count < std::size(regions)) {
      regions[count++] = boot::MemoryRegion{type_out, base, len};
    }

    auto* bytes = reinterpret_cast<uint8_t*>(entry);
    entry = reinterpret_cast<Entry*>(bytes + mmap->entry_size);
  }

  ctx.memory_regions = count;
  ctx.memory_map = regions;
}

i386::mem::Paging* setup_paging(uint32_t mb2_info_addr, boot::BootContext& ctx) noexcept {
  uint32_t bump = first_free_paddr(mb2_info_addr);
  uint32_t early_limit = choose_early_identity_limit(bump);
  EarlyPaging early = setup_early_identity_paging(bump, early_limit);
  enable_paging(early.pd_phys);

  uint32_t managed_base = 0;
  uint32_t managed_end = early_limit;
  size_t managed_frames = static_cast<size_t>(managed_end / PageSize);
  size_t bitmap_bytes = (managed_frames + 7) / 8;

  bump = align_up_4k(bump);
  void* bitmap_storage = reinterpret_cast<void*>(static_cast<uintptr_t>(bump));
  bump += static_cast<uint32_t>(align_up_4k(static_cast<uint32_t>(bitmap_bytes)));

  static mem::builtin::BmPageFrameAllocator pfa;
  pfa.init(bitmap_storage, bitmap_bytes, managed_base, managed_frames);

  for (size_t i = 0; i < ctx.memory_regions; ++i) {
    const auto& r = ctx.memory_map[i];
    if (r.type == boot::MemoryRegionType::Usable) {
      pfa.add_usable_range(r.addr, r.addr + r.length);
    }
  }

  // Force nullptr to be invalid
  pfa.reserve_range(0x000000u, PageSize);

  uint32_t kernel_start = 1 * mem::MiB;
  uint32_t kernel_end =
      static_cast<uint32_t>(reinterpret_cast<uintptr_t>(&_ld_kernel_end_addr));
  pfa.reserve_range(kernel_start, kernel_end - kernel_start);

  uint32_t mb2_end = mb2_end_addr(mb2_info_addr);
  pfa.reserve_range(mb2_info_addr, mb2_end - mb2_info_addr);

  uintptr_t early_used_begin = early.pd_phys;
  uintptr_t early_used_end = bump;
  pfa.reserve_range(early_used_begin, early_used_end - early_used_begin);

  auto bm_phys = reinterpret_cast<uintptr_t>(bitmap_storage);
  pfa.reserve_range(bm_phys, bitmap_bytes);

  static i386::mem::Paging paging{pfa};
  if (!paging.init_kernel_space()) { panic("Initializing kernel space failed."); }

  ctx.ram_start_addr = bump;
  return &paging;
}

void map_framebuffer(kernel::KernelServices& serv) noexcept {
  uintptr_t fb_phys = reinterpret_cast<uintptr_t>(serv.framebuffer->begin());
  size_t fb_bytes = serv.framebuffer->get_pitch() * serv.framebuffer->get_height();
  uintptr_t fb_base = fb_phys & ~uintptr_t(0xFFF);
  uintptr_t fb_end = (fb_phys + fb_bytes + 0xFFF) & ~uintptr_t(0xFFF);
  size_t fb_pages = (fb_end - fb_base) / 4096;
  if (!serv.paging->map_range(fb_base, fb_base, fb_pages,
                              hal::PageFlags::Writable | hal::PageFlags::CacheDisable |
                                  hal::PageFlags::WriteThrough)) {
    panic("Failed to map framebuffer");
  }
  serv.paging->flush_all();
}

void make_basic_mem(boot::BootContext& ctx) noexcept {
  mb2::BasicMemInfoTag inf;
  if (load_tag<mb2::TagType::BasicMeminfo>(inf)) { ctx.upper_mem_kb = inf.mem_upper; }
}

}  // namespace

extern "C" void kmain(uint32_t mb2_info_addr) {
  boot::BootContext ctx{};
  kernel::KernelServices serv{};

  auto& drv_core = ::drv::core();

  ctx.arch = boot::ArchKind::X86;
  ctx.bootloader = boot::BootLoaderKind::Multiboot2;

  // preload all tags:
  (void)mb2::get_tag_map(mb2_info_addr);

  board::pc::register_board_devices(drv_core);
  x86::drv::register_arch_drivers(drv_core);
  drv_core.bind_all();

  serv.keyboard = &x86::input::PS2Keyboard::get_instance();

  serv.serial = find_console(drv_core);
  serv.serial->init();
  auto* serial_sink = setup_logging(*serv.serial);
  logging::backend::set_sink(serial_sink);

  setup_boot_fb(serv);
  make_basic_mem(ctx);
  make_mem_map(ctx);

  const char* str;
  if (mb2::get_cmdline(&str)) {
    ctx.cmdline = str;
  } else {
    ctx.cmdline = "<none>";
  }
  if (mb2::get_bootloader_name(&str))
    ctx.bootloader_name = str;
  else
    ctx.bootloader_name = "Unknown";

  serv.paging = setup_paging(mb2_info_addr, ctx);
  map_framebuffer(serv);

  auto* kernel_heap = mem::get_heap<mem::builtin::BmHeap>();
  mem::init_heap(kernel_heap, ctx.ram_start_addr, 32 * mem::MiB);
  mem::set_kernel_heap(*kernel_heap);

  kernel::Kernel kernel{serv, ctx};
  kernel.enter();

  logging::backend::set_sink(serial_sink);
  panic("Unexpected kernel termination.");
}
