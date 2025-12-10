#pragma once
#include <cstddef>
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

  explicit constexpr Canvas(hal::Framebuffer& fb) : fb(fb) {}

  uint32_t width() const noexcept { return fb.get_width(); }

  uint32_t height() const noexcept { return fb.get_height(); }

  void clear(Color color) noexcept;
  void clear(Color color, Rect area) noexcept;

  void draw_pixel(uint32_t x, uint32_t y, Color c) noexcept;

  void draw_byte(uint32_t x, uint32_t y, uint8_t b, Color c,
                 Color nc = Color::None()) noexcept;

  void draw_buffer(uint32_t x, uint32_t y, uint8_t* b, size_t l, Color c,
                   Color nc = Color::None()) noexcept;

  void draw_rect(Rect rect, Color color) noexcept;

 private:
  hal::Framebuffer& fb;
  const size_t bytes_pre_pixle = fb.get_bpp() / 8;
  const Rect fb_rect{0, 0, fb.get_width(), fb.get_height()};

  uint8_t* get(size_t x, size_t y);
  uint32_t* get32(size_t x, size_t y);

  // no memcpy to ensure we always get the biggest possible size
  void fitcpy(uint32_t* dest, const uint32_t* src, size_t len);
  void fitcpy(uint8_t* dest, const uint8_t* src, size_t len);
  void fitset(uint32_t* dest, uint32_t val, size_t len);
};
}  // namespace gfx
