#include "gfx/canvas.hpp"

#include <cstdint>

#include "gfx/shapes.hpp"

namespace gfx {

void Canvas::draw_rect(Rect rect, uint32_t argb) noexcept {
  const auto xmax = rect.x + rect.w;
  const auto ymax = rect.y + rect.h;
  for (uint32_t y = rect.y; y < ymax; ++y) {
    for (uint32_t x = rect.x; x < xmax; ++x) {
      fb.put_pixel(x, y, argb);
    }
  }
}

void Canvas::draw_border(Rect o, Rect i, uint32_t argb) noexcept {
  const auto xmax = o.x + o.w;
  const auto ymax = o.y + o.h;

  for (uint32_t y = o.y; y < ymax; ++y) {
    if (y < i.y || y > i.y + i.h) {
      for (uint32_t x = o.x; x < xmax; ++x) {
        if (x < i.x || x > i.x + i.w) {
          fb.put_pixel(x, y, argb);
        }
      }
    }
  }
}

void Canvas::draw_border(Rect r, int16_t thickness, uint32_t argb) noexcept {
  Rect r2{
      .x = r.x - thickness,
      .y = r.y - thickness,
      .w = r.w + thickness,
      .h = r.h + thickness,
  };

  if (thickness > 0) {
    draw_border(r2, r, argb);
  } else {
    draw_border(r, r2, argb);
  }
}
}  // namespace gfx
