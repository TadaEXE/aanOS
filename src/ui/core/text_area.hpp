#pragma once

#include "gfx/text/text.hpp"
#include "kernel/containers/gap_buffer.hpp"

namespace ui {
class TextArea {
 public:
  TextArea(gfx::Rect area, gfx::Canvas& canvas, gfx::text::Style style)
      : area(area), canvas(canvas), tr(canvas, style), style(style) {}

  void put_char(char c);
  void put_text(const char* text);
  void remove_last();
  void remove_next();
  void redraw();

  void move_cursor(gfx::Point np);

  void scroll_up(size_t s);
  void scroll_down(size_t s);

 private:
  void draw_rel() {

  }
  gfx::Rect area;
  gfx::Canvas& canvas;
  gfx::text::TextRenderer tr;
  gfx::text::Style style;
  gfx::Point text_cursor{0, 0};
  ctr::GapBuffer<char, 16> buffer;
  size_t lines{0};
};
}  // namespace ui
