#include "ui/tty/tty_text_area.hpp"

namespace ui {

void TtyTextArea::move_left(size_t amount) noexcept {
  auto pos = area.target_pos();
  pos.x -= amount >= pos.x ? pos.x : amount;
  area.move_cursor(pos);
}

void TtyTextArea::move_right(size_t amount) noexcept {
  auto pos = area.target_pos();
  pos.x += amount;
  area.move_cursor(pos);
}

void TtyTextArea::move_up(size_t amount) noexcept {
  auto pos = area.target_pos();
  pos.y -= amount >= pos.y ? pos.y : amount;
  area.move_cursor(pos);
}

void TtyTextArea::move_down(size_t amount) noexcept {
  auto pos = area.target_pos();
  pos.y += amount;
  area.move_cursor(pos);
}

void TtyTextArea::move_line_end() noexcept {
  auto pos = area.cursor_pos();
  pos.x = area.length_of_line(pos.y);
  area.move_cursor(pos);
}

void TtyTextArea::move_end() noexcept {
  gfx::Point pos{};
  pos.y = area.line_count() - 1;
  pos.x = area.length_of_line(pos.y);
  area.move_cursor(pos);
}
}  // namespace ui
