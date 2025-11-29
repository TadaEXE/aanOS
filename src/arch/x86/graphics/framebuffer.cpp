#include "arch/x86/graphics/framebuffer.hpp"

namespace x86::graphics {

void Framebuffer::clear(uint32_t argb) noexcept {
  if (!valid())
    return;

  for (uint32_t y = 0; y < height; ++y) {
    auto* row = addr + y * pitch;
    auto* px = reinterpret_cast<uint32_t*>(row);
    for (uint32_t x = 0; x < width; ++x) {
      px[x] = argb;
    }
  }
}

void Framebuffer::put_pixel(uint32_t x, uint32_t y, uint32_t argb) noexcept {
  if (!valid())
    return;
  if (x >= width || y >= height)
    return;

  auto* row = addr + y * pitch;
  auto* px = reinterpret_cast<uint32_t*>(row);
  px[x] = argb;
}
}  // namespace x86::graphics
