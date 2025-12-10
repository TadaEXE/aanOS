#pragma once

#include "tty/display.hpp"
#include "ui/core/text_area.hpp"

namespace ui {

class TextLineFrame {
 public:
  TextLineFrame(gfx::Rect area, gfx::Canvas& canvas, gfx::text::Style style)
      : area(area), canvas(canvas), tr(canvas, style), style(style) {}

 private:
  gfx::Rect area;
  gfx::Canvas& canvas;
  gfx::text::TextRenderer tr;
  gfx::text::Style style;

};
}  // namespace ui
