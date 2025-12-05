#pragma once
#include <cstdint>

#include "gfx/canvas.hpp"

namespace ui {

class Widget {
 public:
  virtual ~Widget() = default;
  Widget() = default;

  Widget(const Widget&) = default;
  Widget(Widget&&) = delete;
  Widget& operator=(const Widget&) = default;
  Widget& operator=(Widget&&) = delete;

  Widget(uint32_t x, uint32_t y) : x(x), y(y) {}

  virtual void draw(gfx::Canvas& canvas) noexcept = 0;

  virtual void set_position(uint32_t x, uint32_t y) noexcept {
    this->x = x;
    this->y = y;
  }

 protected:
  uint32_t x{0};
  uint32_t y{0};
};
}  // namespace ui
