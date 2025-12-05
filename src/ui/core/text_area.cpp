#include "ui/core/text_area.hpp"

namespace ui {

void TextArea::put_char(char c) {
  buffer.insert(&c);
  if (c == '\n' || c == '\r') ++lines;
}

void TextArea::put_text(const char* text) {
  if (!text) return;

  for (const char* c = text; *c; c++) {
    put_char(*c);
  }
}

void TextArea::remove_last() {
  buffer.delete_left();
}

void TextArea::remove_next() {
  buffer.delete_left();
}

void TextArea::move_cursor(gfx::Point np) {
  buffer.move_to(np.x);
}

void TextArea::redraw() {
  canvas.clear(style.bg, area);
  text_cursor = {0, 0};

  auto px = area.x;
  auto py = area.y;
  tr.set_pos(px, py);

  for (size_t i = 0; i < buffer.count(); ++i) {
    auto g = buffer[i];
    text_cursor.x++;
    if (g == '\n' || g == '\r') {
      py += tr.font.glyph_height + 1;
      tr.set_pos(px, py);
      text_cursor.x = 0;
      text_cursor.y++;
      continue;
    }

    if (px + ((text_cursor.x + style.gap_x) * tr.font.glyph_width * style.scale) >=
        area.end_x()) {
      py += tr.font.glyph_height;
      tr.set_pos(px, py);
      text_cursor.x = 1;
      text_cursor.y++;
    }
    tr.draw_glyph(g);
  }
}

}  // namespace ui
