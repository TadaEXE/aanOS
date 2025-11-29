#include "hal/boot.hpp"

#include "arch/x86/boot/multiboot2.hpp"
#include "arch/x86/graphics/framebuffer.hpp"

namespace hal {

using namespace x86;

void run_arch_boot(int argc, char** argv) {
  (void)argv;

  static graphics::Framebuffer boot_fb;
  multiboot2::FramebufferInfo fi{};

  if (multiboot2::find_framebuffer(argc, fi)) {
    boot_fb.init(fi);

    if (boot_fb.valid()) {
      BootFramebuffer::instance().set(&boot_fb);
    }
  }
}
}  // namespace hal
