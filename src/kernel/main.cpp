#include <cstdint>

// #include "/boot/multiboot2.hpp"
#include "hw/console/text_mode_console.hpp"
#include "hw/graphics/framebuffer.hpp"
#include "input/editor.hpp"
#include "logging/logging.hpp"
#include "ui/window.hpp"

extern "C" void kmain(uint32_t mb2_info_addr) {
  log::msg("editOS starting...");

  hw::multiboot2::FramebufferInfo fi{};
  if (hw::multiboot2::find_framebuffer(mb2_info_addr, fi)) {
    x86::graphics::Framebuffer fb{fi};

    if (fb.valid()) {
      fb.clear(0xFFFF00FF);

      for (uint32_t x = 0; x < fb.get_width(); ++x) {
        fb.put_pixel(x, fb.get_height() / 2, 0xFFAAAAAF);
      }
      for (uint32_t y = 0; y < fb.get_height(); ++y) {
        fb.put_pixel(fb.get_width() / 2, y, 0xFFFEEEEF);
      }

      return;
      for (;;)
        ;
    }
  }

  hw::console::TextModeConsole console;
  console.init();
  const char* msg = "No suitable framebuffer from Multiboot2, falling back to text editor.\n";
  for (const char* p = msg; *p; ++p) {
    console.write_char(*p);
  }

  input::Editor editor;
  editor.run();
}
