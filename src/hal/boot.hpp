#pragma once
#include <optional>

#include "hal/framebuffer.hpp"

namespace hal {

void run_arch_boot(int argc, char* argv[]);
std::optional<Framebuffer> get_boot_framebuffer();

class BootFramebuffer {
 public:
  static BootFramebuffer& instance() {
    static BootFramebuffer instance;
    return instance;
  }

  BootFramebuffer(const BootFramebuffer&) = delete;
  void operator=(const BootFramebuffer&) = delete;

  bool available() { return boot_fb != nullptr; }

  Framebuffer* get() { return boot_fb; }

  void set(Framebuffer* fb) { boot_fb = fb; }

 private:
  BootFramebuffer() {}

  Framebuffer* boot_fb;
};

}  // namespace hal
