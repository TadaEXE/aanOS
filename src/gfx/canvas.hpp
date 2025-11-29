#pragma once
#include <cstdint>

#include "gfx/shapes.hpp"
#include "hal/framebuffer.hpp"

namespace gfx {

class Canvas {
 public:
  explicit Canvas(hal::Framebuffer& fb) : fb(fb) {}

  uint32_t width() const noexcept { return fb.get_width(); }

  uint32_t height() const noexcept { return fb.get_height(); }

  void clear(uint32_t argb) noexcept { fb.clear(argb); }

  void draw_pixel(uint32_t x, uint32_t y, uint32_t argb) noexcept { fb.put_pixel(x, y, argb); }

  void draw_rect(Rect rect, uint32_t argb) noexcept;

  void draw_border(Rect outer, Rect inner, uint32_t argb) noexcept;
  void draw_border(Rect rect, int16_t thickness, uint32_t argb) noexcept;

 private:
  hal::Framebuffer& fb;
};
}  // namespace gfx
