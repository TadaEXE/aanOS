#include <cstring>

#include "gfx/canvas.hpp"
#include "gfx/shapes.hpp"
#include "gfx/text/text.hpp"
#include "hal/boot.hpp"
#include "hal/keyboard.hpp"
#include "input/keymap_us.hpp"
#include "kernel/log.hpp"
#include "kernel/memory/builtin/bm_heap.hpp"
#include "kernel/memory/byte_conversion.hpp"
#include "kernel/memory/heap.hpp"
#include "kernel/panic.hpp"
#include "logging/backend/callback.hpp"
#include "logging/backend/serial.hpp"
#include "logging/logging.hpp"
#include "ui/core/text_area.hpp"
#include "ui/core/window.hpp"

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

logging::backend::LoggingSink* make_gui_logging(gfx::Canvas& can) {
  gfx::Rect logging_area{can.width() - 400, 0, 400, can.height()};
  logging_area -= 10;

  ui::Window win{logging_area};
  win.draw(can);

  gfx::text::Style style{gfx::Color::Black(), gfx::Color::White(), false, 1, -2};
  static ui::TextArea logging_output{logging_area, can, style};
  static logging::backend::CallbackSink cb_sink(
      [](char c, void* ctx) {
        auto* ta = reinterpret_cast<ui::TextArea*>(ctx);
        ta->put_char(c);
        ta->redraw();
      },
      &logging_output);

  return &cb_sink;
}
}  // namespace

[[noreturn]] void hal::enter_kernel(boot::BootContext& ctx) {
  if (!kernel_fuse()) panic("Entry fuse blew");
  logging::backend::set_sink(setup_logging(ctx));

  log_msg("editOS kernel entered...");
  log_msg("Booted by %s", ctx.bootloader_name);
  log_msg("Bootoptions: %s", ctx.cmdline);
  log_msg("%d KiB available in upper memory", ctx.upper_mem_kb);
  log_msg("Kernel size %d KiB (~%d%%)", ctx.ram_start_addr - ctx.upper_mem_start,
          ((ctx.ram_start_addr - ctx.upper_mem_start) * 100) / ctx.upper_mem_kb);

  if (ctx.memory_map && ctx.memory_regions) {
    log_msg("Found memory map (%d entries):", ctx.memory_regions);
    for (size_t i = 0; i < ctx.memory_regions; ++i) {
      auto e = ctx.memory_map[i];
      log_msg("  %d MiB at %x [%s]", mem::B_to_MiB(e.length), e.addr,
              boot::MemoryRegionTypeName(e.type));
    }

    auto* kernel_heap = mem::get_heap<mem::builtin::BmHeap>();
    mem::init_heap(kernel_heap, ctx.ram_start_addr, 32 * mem::MiB);
    mem::set_kernel_heap(*kernel_heap);
    log_msg("Kernel heap (%d MiB) initialized at %p", 32, ctx.ram_start_addr);
  }

  hal::Framebuffer* fb = ctx.boot_framebuffer;
  if (!fb) { panic("No framebuffer provided by bootloader. Abort!"); }

  gfx::Canvas can(*fb);
  can.clear(0xFF202040);

  auto& kb = hal::keyboard();
  gfx::Rect terminal_area{0, 0, can.width(), can.height()};
  terminal_area -= 10;

  ui::Window terminal_win{terminal_area};
  terminal_win.draw(can);

  gfx::text::Style style{gfx::Color::Black(), gfx::Color::White(), false, 1, -2};
  ui::TextArea terminal{terminal_area, can, style};
  terminal.put_text("#>");
  terminal.redraw();

  for (;;) {
    hal::KeyEvent ev{};
    char c;
    if (kb.poll(ev)) {
      log_msg("Got key event %o", &ev);
      if (ev.key == Key::Backspace && ev.type == hal::KeyEventType::Press) {
        terminal.remove_last();
      } else if (input::key_event_to_char(ev, c)) {
        terminal.put_char(c);
      }
      terminal.redraw();
    }
  }
}
