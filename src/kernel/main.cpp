#include <cstring>

#include "gfx/canvas.hpp"
#include "gfx/shapes.hpp"
#include "gfx/text/text.hpp"
#include "hal/boot.hpp"
#include "hal/keyboard.hpp"
#include "kernel/log.hpp"
#include "kernel/memory/byte_conversion.hpp"
#include "kernel/panic.hpp"
#include "kernel/shell/shell.hpp"
#include "kernel/tty/tty.hpp"
#include "logging/backend/serial.hpp"
#include "logging/logging.hpp"
#include "ui/core/text_area.hpp"
#include "ui/core/window.hpp"
#include "ui/tty/tty_text_area.hpp"

namespace {
bool kernel_fuse() {
  static bool fuse{false};
  if (fuse) {
    log_msg("Tried to enter kernel (hal::enter_kernel()) more than once!");
    return false;
  }
  return true;
}

logging::backend::LoggingSink* setup_logging(boot::BootContext& ctx) {
  if (ctx.system_serial_bus) {
    static logging::backend::SerialSink serial_sink(*ctx.system_serial_bus);
    log_msg("");
    log_msg("");
    log_msg("");
    return &serial_sink;
  }
  return nullptr;
}

}  // namespace

[[noreturn]] void hal::enter_kernel(boot::BootContext& ctx) {
  if (!kernel_fuse()) panic("Entry fuse blew");
  logging::backend::set_sink(setup_logging(ctx));

  log_msg("editOS kernel entered...");
  log_msg("Booted by %s", ctx.bootloader_name);
  log_msg("Bootoptions: %s", ctx.cmdline);
  log_msg("%d KiB available in upper memory", ctx.upper_mem_kb);
  log_msg("Kernel size %d MiB (~%d%%)",
          mem::KiB_to_MiB(ctx.ram_start_addr - ctx.upper_mem_start),
          ((ctx.ram_start_addr - ctx.upper_mem_start) * 100) / ctx.upper_mem_kb);

  if (ctx.memory_map && ctx.memory_regions) {
    log_msg("Found memory map (%d entries):", ctx.memory_regions);
    for (size_t i = 0; i < ctx.memory_regions; ++i) {
      auto e = ctx.memory_map[i];
      log_msg("  %d MiB at %x [%s]", mem::B_to_MiB(e.length), e.addr,
              boot::MemoryRegionTypeName(e.type));
    }

    log_msg("Kernel heap (%d MiB) initialized at %p", 32, ctx.ram_start_addr);
  }

  hal::IFramebuffer* fb = ctx.boot_framebuffer;
  if (!fb) { panic("No framebuffer provided by bootloader. Abort!"); }

  gfx::Canvas can(*fb);
  can.clear(0xFF202040);

  auto& kb = hal::Keyboard::get();
  gfx::Rect tty_rect{0, 0, can.width(), can.height()};
  tty_rect -= 10;

  ui::Window tty_win{tty_rect};
  tty_win.draw(can);

  gfx::text::Style style{gfx::Color::Black(), gfx::Color::White(), false, 1, -2, 1};
  ui::TextArea tty_area{tty_rect, can, style};
  ui::TtyTextArea tty_display(tty_area);
  tty::Tty tty{tty_display, kb};
  shell::Shell shell{tty};
  shell.register_builtin_commands();

  shell.run();
}
