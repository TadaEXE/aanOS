#include "ui/core/text_area.hpp"

namespace ui {

void TextArea::put_char(char c) noexcept {
  buffer.insert(&c);
  ++target_cursor.x;
  if (c == '\n' || c == '\r') {
    ++target_cursor.y;
    target_cursor.x = 0;
    ++lines;
  }
}

void TextArea::put_text(const char* text) noexcept {
  if (!text) return;

  for (const char* c = text; *c; c++) {
    put_char(*c);
  }
}

void TextArea::remove_last() noexcept {
  buffer.delete_left();
  --target_cursor.x;
}

void TextArea::remove_next() noexcept {
  buffer.delete_left();
}

void TextArea::move_cursor(gfx::Point np) noexcept {
  if (np.y >= lines) np.y = lines;

  const auto start = line_start_index(np.y);
  const auto end = line_end_index(start);
  const auto line_len = (end + 1) - start;

  auto idx = np.x > line_len ? end : start + np.x;
  buffer.move_to(idx);
  target_cursor = {idx - start, np.y};
}

void TextArea::redraw() noexcept {
  canvas.clear(style.bg, area);
  gfx::Point draw_pos{0, 0};

  const auto cap = visible_line_capacity();
  if (cap == 0) return;

  const auto total = count_lines();
  if (total == 0) return;

  const auto bottom_start = total > cap ? total - cap : 0;

  if (follow_bottom) {
    first_visible_line = bottom_start;
  } else {
    if (total <= cap) {
      first_visible_line = 0;
      follow_bottom = true;
    } else if (first_visible_line > bottom_start) {
      first_visible_line = bottom_start;
      follow_bottom = true;
    }
  }

  const auto start_index = line_start_index(first_visible_line);
  const auto n = buffer.count();

  uint32_t px = area.x;
  uint32_t py = area.y;
  tr.set_pos(px, py);

  const auto gw = eff_glyph_width();
  const auto gh = line_height();

  size_t current_line = 0;

  real_cursor = target_cursor;

  for (size_t i = start_index; i < n; ++i) {
    char g = buffer[i];

    if (g == '\n') {
      ++current_line;
      if (current_line >= cap) break;

      draw_pos.x = 0;
      ++draw_pos.y;
      py += gh;
      tr.set_pos(px, py);
      continue;
    }

    auto next_x = px + (gw * draw_pos.x);
    // This causes a bug where we won't be scrolled all the way to the bottom,
    // because we are not counting the number of linewraps among the visible lines beforehand.
    // TODO: Fix this later
    if (next_x >= area.end_x()) {
      break;

      ++current_line;
      if (current_line >= cap) break;

      draw_pos.x = 0;
      ++draw_pos.y;
      py += gh;
      tr.set_pos(px, py);
    }

    if (i + 1 < n && draw_pos.y == target_cursor.y && buffer[i + 1] == '\n' &&
        draw_pos.x < real_cursor.x) {
      real_cursor.x = draw_pos.x;
    }

    tr.draw_glyph(g, draw_pos == real_cursor);
    ++draw_pos.x;
  }

  if (draw_pos == real_cursor) { tr.draw_glyph(' ', true); }
}

void TextArea::scroll_up(size_t amount) {
  if (amount == 0) return;

  const auto total = count_lines();
  const auto cap = visible_line_capacity();

  if (cap == 0 || total <= cap) return;

  if (amount > first_visible_line) {
    first_visible_line = 0;
  } else {
    first_visible_line -= amount;
  }

  follow_bottom = false;
}

void TextArea::scroll_down(size_t amount) {
  if (amount == 0) return;

  const auto total = count_lines();
  const auto cap = visible_line_capacity();

  if (cap == 0) return;

  if (total <= cap) {
    first_visible_line = 0;
    follow_bottom = true;
    return;
  }

  const auto bottom_start = total - cap;
  auto new_first = first_visible_line + amount;

  if (new_first >= bottom_start) {
    first_visible_line = bottom_start;
    follow_bottom = true;
  } else {
    first_visible_line = new_first;
    follow_bottom = false;
  }
}

size_t TextArea::length_of_line(size_t line) const noexcept {
  const auto start = line_start_index(line);
  const auto end = line_end_index(start);
  return end - start;
}

size_t TextArea::visible_line_capacity() const noexcept {
  if (area.h == 0) return 0;

  const auto lh = line_height();
  if (lh == 0) return 0;

  return static_cast<size_t>(area.h / lh);
}

size_t TextArea::count_lines() noexcept {
  const auto n = buffer.count();
  if (n == 0) {
    lines = 1;
    return lines;
  }

  size_t count = 1;
  for (size_t i = 0; i < n; ++i) {
    if (buffer[i] == '\n') { ++count; }
  }

  lines = count;
  return lines;
}

size_t TextArea::line_start_index(size_t line) const noexcept {
  if (line == 0) return 0;

  const auto n = buffer.count();
  size_t cur = 0;

  for (size_t i = 0; i < n; ++i) {
    if (buffer[i] == '\n') {
      ++cur;
      if (cur == line) { return i + 1; }
    }
  }

  return n;
}

size_t TextArea::line_end_index(size_t start_idx) const noexcept {
  const auto n = buffer.count();

  for (size_t i = start_idx; i < n; ++i) {
    if (buffer[i] == '\n') return i;
  }

  return n;
}

size_t TextArea::line_height() const noexcept {
  return (tr.font.glyph_height + style.gap_y) * style.scale;
}

size_t TextArea::eff_glyph_width() const noexcept {
  return (tr.font.glyph_width + style.gap_x) * style.scale;
}

size_t TextArea::max_line_length() const noexcept {
  return area.w / eff_glyph_width();
}

}  // namespace ui
