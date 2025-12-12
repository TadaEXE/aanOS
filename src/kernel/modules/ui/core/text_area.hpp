#pragma once
#include <cstddef>

#include "containers/gap_buffer.hpp"
#include "gfx/canvas.hpp"
#include "gfx/shapes.hpp"
#include "gfx/text/textrenderer.hpp"

namespace ui {
class TextArea {
 public:
  TextArea(const TextArea&) = delete;
  TextArea(TextArea&&) = delete;
  TextArea& operator=(const TextArea&) = delete;
  TextArea& operator=(TextArea&&) = delete;

  TextArea(gfx::Rect area, gfx::text::TextRenderer& tr, gfx::text::Style style)
      : area(area), /*canvas(canvas),*/ tr(tr), style(style) {}

  void put_char(char c) noexcept;
  void put_text(const char* text) noexcept;

  void remove_last() noexcept;
  void remove_next() noexcept;

  void redraw() noexcept;

  void move_cursor(gfx::Point np) noexcept;

  void scroll_up(size_t amount = 1);
  void scroll_down(size_t amount = 1);

  gfx::Point target_pos() const noexcept { return target_cursor; }

  gfx::Point cursor_pos() const noexcept { return real_cursor; }

  size_t current_line_length() const noexcept;
  size_t length_of_line(size_t line) const noexcept;

  size_t line_count() const noexcept { return lines; }

  size_t get_glyphs_per_line() const noexcept;

 private:
  size_t visible_line_capacity() const noexcept;
  size_t count_lines() noexcept;
  size_t line_start_index(size_t line) const noexcept;
  size_t line_end_index(size_t start_idx) const noexcept;
  size_t line_height() const noexcept;
  size_t eff_glyph_width() const noexcept;

  gfx::Rect area;
  // gfx::Canvas& canvas;
  gfx::text::TextRenderer& tr;
  gfx::text::Style style;

  gfx::Point real_cursor{0, 0};
  gfx::Point target_cursor{0, 0};

  ctr::GapBuffer<char, 16> buffer{};

  size_t lines{0};
  size_t first_visible_line{0};

  bool follow_bottom{true};
};
}  // namespace ui
