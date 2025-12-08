#pragma once

#include <cstdint>

#include "gfx/color.hpp"
#include "gfx/shapes.hpp"

namespace hal {

class IFramebuffer {
 public:
  virtual ~IFramebuffer() = default;

  virtual bool valid() const noexcept = 0;
  virtual void clear(gfx::Color color, gfx::Rect area = gfx::Rect::Empty()) noexcept = 0;
  virtual void put_pixel(uint32_t x, uint32_t y, gfx::Color color) noexcept = 0;
  virtual uint32_t get_width() const noexcept = 0;
  virtual uint32_t get_height() const noexcept = 0;
};

}  // namespace hal
