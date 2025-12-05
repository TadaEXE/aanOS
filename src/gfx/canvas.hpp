#pragma once
#include <cstdint>

#include "gfx/color.hpp"
#include "gfx/shapes.hpp"
#include "hal/framebuffer.hpp"

namespace gfx {

class Canvas {
 public:
  Canvas(const Canvas&) = delete;
  Canvas(Canvas&&) = delete;
  Canvas& operator=(const Canvas&) = delete;
  Canvas& operator=(Canvas&&) = delete;

  explicit Canvas(hal::Framebuffer& fb) : fb(fb) {}

  uint32_t width() const noexcept { return fb.get_width(); }

  uint32_t height() const noexcept { return fb.get_height(); }

  void clear(Color color, Rect area = Rect::Empty()) noexcept { fb.clear(color, area); }

  void draw_pixel(uint32_t x, uint32_t y, Color color) noexcept;

  void draw_rect(Rect rect, Color color) noexcept;

  void draw_border(Rect outer, Rect inner, Color color) noexcept;
  void draw_border(Rect rect, int16_t thickness, Color color) noexcept;

 private:
  hal::Framebuffer& fb;
};
}  // namespace gfx
