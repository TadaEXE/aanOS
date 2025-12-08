#pragma once

#include <cstdint>

#include "gfx/color.hpp"
#include "gfx/shapes.hpp"
#include "hal/framebuffer.hpp"
#include "kernel/boot/multiboot2.hpp"

namespace x86::graphics {

class Framebuffer : public hal::IFramebuffer {
 public:
  Framebuffer() = default;

  Framebuffer(const Framebuffer&) = delete;
  Framebuffer(Framebuffer&&) = delete;
  Framebuffer& operator=(const Framebuffer&) = default;
  Framebuffer& operator=(Framebuffer&&) = default;

  explicit Framebuffer(const mb2::FramebufferTag& info)
      : addr(reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(info.addr))),
        pitch(info.pitch),
        width(info.width),
        height(info.height),
        bpp(info.bpp),
        type_fb(info.type_fb) {}

  bool valid() const noexcept override {
    return addr != nullptr && bpp == 32 && width > 0 && height > 0 && pitch >= width * 4;
  }

  void clear(gfx::Color color, gfx::Rect area = gfx::Rect::Empty()) noexcept override;
  void put_pixel(uint32_t x, uint32_t y, gfx::Color color) noexcept override;

  uint32_t get_width() const noexcept override { return width; }

  uint32_t get_height() const noexcept override { return height; }

 private:
  uint8_t* addr{nullptr};
  uint32_t pitch{0};
  uint32_t width{0};
  uint32_t height{0};
  uint8_t bpp{0};
  uint8_t type_fb{0};
};
}  // namespace x86::graphics
