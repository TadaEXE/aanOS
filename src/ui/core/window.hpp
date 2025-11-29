#pragma once
#include <cstdint>
#include "gfx/canvas.hpp"
#include "gfx/shapes.hpp"
#include "ui/widget.hpp"

namespace ui {

class Window : public Widget {
 public:
  Window(uint32_t w, uint32_t h) : rect(gfx::Rect{0, 0, w, h}) {}

  explicit Window(gfx::Rect rect) : Widget(rect.x, rect.y), rect(rect) {}

  void draw(gfx::Canvas& canvas) noexcept override;
  void set_position(uint32_t x, uint32_t y) noexcept override;

 private:
  gfx::Rect rect;

  uint32_t bg_color{0xFF303030};
  uint32_t border_color{0xFF101010};
  uint32_t border_thickness{5};
};
}  // namespace ui
