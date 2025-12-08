#pragma once
#include <optional>

#include "hal/framebuffer.hpp"
#include "kernel/boot/boot_context.hpp"

namespace hal {

[[noreturn]] void enter_kernel(boot::BootContext& ctx);

std::optional<IFramebuffer> get_boot_framebuffer();

class BootFramebuffer {
 public:
  static BootFramebuffer& instance() {
    static BootFramebuffer instance;
    return instance;
  }

  BootFramebuffer(BootFramebuffer&&) = delete;
  BootFramebuffer& operator=(BootFramebuffer&&) = delete;
  BootFramebuffer(const BootFramebuffer&) = delete;
  void operator=(const BootFramebuffer&) = delete;

  bool available() { return boot_fb != nullptr; }

  IFramebuffer* get() { return boot_fb; }

  void set(IFramebuffer* fb) { boot_fb = fb; }

 private:
  BootFramebuffer() {}

  IFramebuffer* boot_fb;
};

}  // namespace hal
