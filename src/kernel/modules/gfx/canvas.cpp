#include "gfx/canvas.hpp"

#include <cstdint>
#include <cstring>

#include <kernel/log.hpp>

#include "color.hpp"
#include "gfx/shapes.hpp"
#include "hal/framebuffer.hpp"

namespace gfx {

constexpr void expand(uint32_t out[8], uint8_t b, Color c, Color c2 = Color::None()) {
  for (auto i = 0; i < 8; ++i) {
    out[i] = (b & (0b10000000 >> i)) == 0 ? static_cast<uint32_t>(c2)
                                          : static_cast<uint32_t>(c);
  }
}

void Canvas::clear(Color color) noexcept {
  draw_rect(fb_rect, color);
}

void Canvas::clear(Color color, Rect rect) noexcept {
  draw_rect(rect, color);
}

void Canvas::draw_pixel(uint32_t x, uint32_t y, Color c) noexcept {
  auto* pixel = get32(x, y);
  *pixel = static_cast<uint32_t>(c);
}

void Canvas::draw_byte(uint32_t x, uint32_t y, uint8_t b, Color c, Color nc) noexcept {
  uint32_t buf[8];
  expand(buf, b, c, nc);
  auto* pixel = get32(x, y);
  fitcpy(pixel, buf, 8);
}

void Canvas::draw_buffer(uint32_t x, uint32_t y, uint8_t* b, size_t l, Color c,
                         Color nc) noexcept {
  if (!b) return;

  auto* pixel = get32(x, y);
  if (reinterpret_cast<uint8_t*>(pixel) + (l * 4) >= fb.end()) return;

  uint32_t buf[8];

  for (size_t i = 0; i < l; ++i) {
    expand(buf, b[i], c, nc);
    fitcpy(pixel, buf, 8);
    pixel += 8;
  }
}

void Canvas::draw_rect(Rect rect, Color color) noexcept {
  const auto ymax = rect.end_y();
  for (uint32_t y = rect.y; y < ymax; ++y) {
    auto* pixel = get32(rect.x, y);
    fitset(pixel, static_cast<uint32_t>(color), rect.w);
  }
}

uint8_t* Canvas::get(size_t x, size_t y) {
  return fb.begin() + y * fb.get_pitch() + x * bytes_pre_pixle;
}

uint32_t* Canvas::get32(size_t x, size_t y) {
  return reinterpret_cast<uint32_t*>(fb.begin() + y * fb.get_pitch()) + x;
}

void Canvas::fitcpy(uint32_t* dest, const uint32_t* src, size_t len) {
  if (!dest || !src) return;
  for (size_t i = 0; i < len; ++i) {
    if (src[i] == Color::None()) continue;

    dest[i] = src[i];
  }
}

void Canvas::fitcpy(uint8_t* dest, const uint8_t* src, size_t len) {
  if (!dest || !src) return;
  if (len % 4 == 0) {
    fitcpy(reinterpret_cast<uint32_t*>(dest), reinterpret_cast<const uint32_t*>(src),
           len / 4);
  } else {
    for (size_t i = 0; i < len; ++i) {
      if (src[i] == Color::None()) continue;
      dest[i] = src[i];
    }
  }
}

void Canvas::fitset(uint32_t* dest, uint32_t val, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    dest[i] = val;
  }
}

}  // namespace gfx
