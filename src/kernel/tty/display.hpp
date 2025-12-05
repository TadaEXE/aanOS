#pragma once

#include <cstddef>

#include "gfx/shapes.hpp"

namespace tty {

class Display {
 public:
  virtual ~Display() = default;
  virtual void put_char(char c) noexcept = 0;
  virtual void backspace() noexcept = 0;
  virtual void flush() noexcept = 0;

  virtual void scroll_up(size_t amount) noexcept = 0;
  virtual void scroll_down(size_t amount) noexcept = 0;

  virtual void move_left(size_t amount) noexcept = 0;
  virtual void move_right(size_t amount) noexcept = 0;

  virtual void move_up(size_t amount) noexcept = 0;
  virtual void move_down(size_t amount) noexcept = 0;

  virtual void move_line_end() noexcept = 0;
  virtual void move_end() noexcept = 0;

  virtual gfx::Point cursor() noexcept = 0;
};
}  // namespace tty
