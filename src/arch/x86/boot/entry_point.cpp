#include <cstdint>
#include <iterator>

#include "arch/x86/graphics/framebuffer.hpp"
#include "arch/x86/io/serial.hpp"
#include "hal/boot.hpp"
#include "kernel/boot/boot_context.hpp"
#include "kernel/log.hpp"
#include "kernel/memory/builtin/bm_heap.hpp"
#include "kernel/memory/heap.hpp"
#include "kernel/panic.hpp"

using namespace x86;

// 4K aligned address after kernel
extern "C" char _ld_kernel_end_addr;

void setup_boot_fb(boot::BootContext& ctx) {
  mb2::FramebufferTag fb_tag{};
  static graphics::Framebuffer boot_fb;

  if (mb2::load_tag<mb2::TagType::Framebuffer>(fb_tag)) {
    boot_fb = graphics::Framebuffer(fb_tag);

    if (boot_fb.valid()) { ctx.boot_framebuffer = &boot_fb; }
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

void make_basic_mem(boot::BootContext& ctx) {
  mb2::BasicMemInfoTag inf;
  if (load_tag<mb2::TagType::BasicMeminfo>(inf)) { ctx.upper_mem_kb = inf.mem_upper; }
}

extern "C" void kmain(uint32_t mb2_info_addr) {
  boot::BootContext ctx{};
  ctx.arch = boot::ArchKind::X86;
  ctx.bootloader = boot::BootLoaderKind::Multiboot2;
  ctx.system_serial_bus = &io::get_serial0();

  // preload all tags:
  auto& map = mb2::get_tag_map(mb2_info_addr);

  setup_boot_fb(ctx);
  make_basic_mem(ctx);
  make_mem_map(ctx);
  ctx.ram_start_addr = reinterpret_cast<uintptr_t>(&_ld_kernel_end_addr);
  auto* kernel_heap = mem::get_heap<mem::builtin::BmHeap>();
  mem::init_heap(kernel_heap, ctx.ram_start_addr, 32 * mem::MiB);
  mem::set_kernel_heap(*kernel_heap);

  const char* str;
  if (mb2::get_cmdline(&str))
    ctx.cmdline = str;
  else
    ctx.cmdline = "";

  if (mb2::get_bootloader_name(&str))
    ctx.bootloader_name = str;
  else
    ctx.bootloader_name = "Unknown";

  hal::enter_kernel(ctx);
  for (size_t i = 0; i < map.size(); ++i) {
    auto& e = map.data[i];
    log_msg("%x = k:%x v:%p", i, static_cast<uint32_t>(e.key), e.value);
  }
  panic("Unexpected kernel termination. %u", 1);
}
