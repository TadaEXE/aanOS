#pragma once

#include <cstdint>

namespace hal {

class Framebuffer {
 public:
  virtual bool valid() const noexcept = 0;
  virtual void clear(uint32_t argb) noexcept = 0;
  virtual void put_pixel(uint32_t x, uint32_t y, uint32_t argb) noexcept = 0;
  virtual uint32_t get_width() const noexcept = 0;
  virtual uint32_t get_height() const noexcept = 0;
};

}  // namespace hal
