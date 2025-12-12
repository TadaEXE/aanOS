#pragma once

#include "tty/display.hpp"
#include "ui/core/text_area.hpp"

namespace ui {

class TtyTextArea : public tty::Display {
 public:
  TtyTextArea(const TtyTextArea&) = delete;
  TtyTextArea(TtyTextArea&&) = delete;
  TtyTextArea& operator=(const TtyTextArea&) = delete;
  TtyTextArea& operator=(TtyTextArea&&) = delete;

  explicit TtyTextArea(TextArea& area) noexcept : area(area) {}

  void put_char(char c) noexcept override { area.put_char(c); }

  void backspace() noexcept override { area.remove_last(); }

  void flush() noexcept override { area.redraw(); }

  void scroll_up(size_t s) noexcept override { area.scroll_up(s); }

  void scroll_down(size_t s) noexcept override { area.scroll_down(s); }

  void move_left(size_t amount) noexcept override;
  void move_right(size_t amount) noexcept override;
  void move_up(size_t amount) noexcept override;
  void move_down(size_t amount) noexcept override;
  void move_line_end() noexcept override;
  void move_end() noexcept override;

  gfx::Point cursor() noexcept override { return area.cursor_pos(); }
  size_t get_line_length() const noexcept override { return area.get_glyphs_per_line(); }

 private:
  TextArea& area;
};

}  // namespace ui
