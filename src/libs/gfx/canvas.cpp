#include "gfx/canvas.hpp"

#include <cstdint>

#include "gfx/shapes.hpp"

namespace gfx {

void Canvas::draw_pixel(uint32_t x, uint32_t y, Color color) noexcept {
  fb.put_pixel(x, y, color);
}

void Canvas::draw_rect(Rect rect, Color color) noexcept {
  const auto xmax = rect.x + rect.w;
  const auto ymax = rect.y + rect.h;
  for (uint32_t y = rect.y; y < ymax; ++y) {
    for (uint32_t x = rect.x; x < xmax; ++x) {
      fb.put_pixel(x, y, color);
    }
  }
}

void Canvas::draw_border(Rect o, Rect i, Color color) noexcept {
  const auto xmax = o.x + o.w;
  const auto ymax = o.y + o.h;

  for (uint32_t y = o.y; y < ymax; ++y) {
    for (uint32_t x = o.x; x < xmax; ++x) {
      if (i.is_inbounds(x, y)) continue;
      fb.put_pixel(x, y, color);
    }
  }
}

void Canvas::draw_border(Rect r, int16_t thickness, Color color) noexcept {
  Rect r2 = r + thickness;

  if (thickness > 0) {
    draw_border(r2, r, color);
  } else {
    draw_border(r, r2, color);
  }
}

// void Canvas::draw_char(uint32_t x, uint32_t y, char c,
//                        const text::Style& style) noexcept {
//   const uint8_t* glyph = text::font5x7_glyph_for(c);
//
//   for (uint8_t row = 0; row < text::FONT5X7_HEIGHT; ++row) {
//     uint8_t bits = glyph[row];
//
//     for (uint8_t col = 0; col < text::FONT5X7_WIDTH; ++col) {
//       bool on = (bits >> (text::FONT5X7_WIDTH - 1 - col)) & 0x1u;
//
//       if (on) {
//         draw_pixel(x + col, y + row, style.text_color);
//       } else if (style.draw_bg) {
//         draw_pixel(x + col, y + row, style.bg_color);
//       }
//     }
//   }
// }
//
// void Canvas::draw_text(uint32_t x, uint32_t y, const char* text,
//                        const text::Style& style) noexcept {
//   uint32_t cx = x;
//   uint32_t cy = y;
//
//   while (*text) {
//     char c = *text++;
//
//     if (c == '\n') {
//       cx = x;
//       cy += text::char_height();
//       continue;
//     }
//
//     draw_char(cx, cy, c, style);
//     cx += text::char_width();
//   }
// }
}  // namespace gfx
