#include "arch/x86/common/graphics/framebuffer.hpp"

#include "gfx/color.hpp"

namespace x86::graphics {

void Framebuffer::clear(gfx::Color color, gfx::Rect area) noexcept {
  if (!valid()) return;

  uint32_t ys = area.y;
  uint32_t ye = area.end_y();
  uint32_t xs = area.x;
  uint32_t xe = area.end_x();

  if (area == gfx::Rect::Empty()) {
    ys = 0;
    ye = height;
    xs = 0;
    xe = width;
  }

  for (uint32_t y = ys; y < ye; ++y) {
    auto* row = addr + y * pitch;
    auto* px = reinterpret_cast<uint32_t*>(row);
    for (uint32_t x = xs; x < xe; ++x) {
      px[x] = color;
    }
  }
}

void Framebuffer::put_pixel(uint32_t x, uint32_t y, gfx::Color color) noexcept {
  if (!valid()) return;
  if (x >= width || y >= height) return;

  auto* row = addr + y * pitch;
  auto* px = reinterpret_cast<uint32_t*>(row);
  px[x] = color;
}
}  // namespace x86::graphics
