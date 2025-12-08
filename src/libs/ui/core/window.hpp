#pragma once
#include <cstdint>

#include "gfx/canvas.hpp"
#include "gfx/color.hpp"
#include "gfx/shapes.hpp"
#include "ui/core/widget.hpp"

namespace ui {

class Window : public Widget {
 public:
  Window(uint32_t w, uint32_t h) : rect(gfx::Rect{0, 0, w, h}) {}

  explicit Window(gfx::Rect rect) : Widget(rect.x, rect.y), rect(rect) {}

  void draw(gfx::Canvas& canvas) noexcept override;
  void set_position(uint32_t x, uint32_t y) noexcept override;

 protected:
  gfx::Rect rect;

  gfx::Color bg_color{gfx::Color::Gray()};
  gfx::Color border_color{gfx::Color::Black()};
  uint32_t border_thickness{5};
};
}  // namespace ui
