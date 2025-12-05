#pragma once

#include <string_view>

#include "hal/keyboard.hpp"
#include "kernel/containers/string.hpp"
#include "kernel/tty/display.hpp"

namespace tty {

class Tty {
 public:
  Tty(const Tty&) = delete;
  Tty(Tty&&) = delete;
  Tty& operator=(const Tty&) = delete;
  Tty& operator=(Tty&&) = delete;

  Tty(Display& display, hal::Keyboard& keyboard) noexcept
      : display(display), keyboard(keyboard) {}

  void write_char(char c) noexcept;

  void write(std::string_view sv) noexcept;
  void write(const ctr::String& s) noexcept;

  void write_line(std::string_view sv) noexcept;
  void write_line(ctr::String& s) noexcept;

  char read_char_blocking() noexcept;

  void readline(ctr::String& out, std::string_view prompt) noexcept;

 private:
  Display& display;
  hal::Keyboard& keyboard;
  bool raw_mode = false;
};

}  // namespace tty
