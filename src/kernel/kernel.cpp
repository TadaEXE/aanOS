#include "kernel.hpp"

#include <cstring>

#include <kernel/log.hpp>
#include <kernel/panic.hpp>

#include "boot/boot_context.hpp"
#include "boot/multiboot2.hpp"
#include "gfx/canvas.hpp"
#include "gfx/shapes.hpp"
#include "gfx/text/textrenderer.hpp"
#include "hal/boot.hpp"
#include "logging/backend/serial.hpp"
#include "logging/logging.hpp"
#include "memory/byte_conversion.hpp"
#include "shell/shell.hpp"
#include "tty/tty.hpp"
#include "ui/core/text_area.hpp"
#include "ui/core/window.hpp"
#include "ui/tty/tty_text_area.hpp"

namespace kernel {
void kernel_fuse_set() {
  static bool fuse{false};
  if (fuse) { panic("Tried to enter kernel more than once!"); }
  fuse = true;
}

logging::backend::LoggingSink* setup_logging(KernelServices& serv) {
  if (serv.serial) {
    static logging::backend::SerialSink serial_sink(*serv.serial);
    log_msg("");
    log_msg("");
    log_msg("");
    return &serial_sink;
  }
  return nullptr;
}

[[noreturn]] void Kernel::enter() {
  kernel_fuse_set();

  logging::backend::set_sink(setup_logging(services));
  auto& map = mb2::get_tag_map();
  for (size_t i = 0; i < map.size(); ++i) {
    auto& e = map.data[i];
    log_msg("%x = k:%x v:%p (Tagsize: %x)", i, static_cast<uint32_t>(e.key), e.value,
            e.value->size);
  }

  log_msg("aanOS kernel entered...");
  log_msg("Booted by %s", ctx.bootloader_name);
  log_msg("Bootoptions: %s", ctx.cmdline);
  log_msg("%d KiB available in upper memory", ctx.upper_mem_kb);

  if (ctx.memory_map && ctx.memory_regions) {
    log_msg("Found memory map (%d entries):", ctx.memory_regions);
    for (size_t i = 0; i < ctx.memory_regions; ++i) {
      auto e = ctx.memory_map[i];
      log_msg("  %d MiB at %x [%s]", mem::B_to_MiB(e.length), e.addr,
              boot::MemoryRegionTypeName(e.type));
    }

    log_msg("Kernel heap (%d MiB) initialized at %p", 32, ctx.ram_start_addr);
  }

  uintptr_t out;
  hal::PageFlags fl;
  if (!services.paging->translate(0xFD000000u, out, fl)) { log_msg("FB not mapped!"); }

  uint8_t* alloc_test = new uint8_t{3};
  log_msg("Can allocate test value %u at addr %p", *alloc_test, alloc_test);

  hal::Framebuffer* fb = services.framebuffer;
  if (!fb) { panic("No framebuffer provided by bootloader. Abort!"); }
  log_msg("Got framebuffer(%p) %o", fb->begin(), fb);

  gfx::Canvas can(*fb);
  can.clear(0xFF202040);

  log_msg("Can write to framebuffer");

  auto* kb = services.keyboard;
  if (!kb) { panic("No keyboard provided by bootloader. Abort!"); }
  gfx::Rect tty_rect{0, 0, can.width(), can.height()};

  ui::Window tty_win{tty_rect};
  tty_win.draw(can);

  gfx::text::Style style{gfx::Color::Black(), gfx::Color::White(), false, 1, 1, 1};
  gfx::text::TextRenderer tr{can, style};
  ui::TextArea tty_area{tty_rect, tr, style};
  ui::TtyTextArea tty_display(tty_area);
  tty::Tty tty{tty_display, *kb};
  shell::Shell shell{tty};
  shell.register_builtin_commands();

  shell.run();
}
}  // namespace kernel
