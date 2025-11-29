#pragma once

#include <cstdint>

#include "ui/core/iconsole.hpp"

namespace x86::console {

class TextModeConsole : ui::IConsole {
 public:
  constexpr TextModeConsole() = default;

  void init() noexcept override;
  void clear() noexcept override;
  void write_char(char c) noexcept override;
  void write_string(const char* s) noexcept override;
  void backspace() noexcept override;

 private:
  uint16_t cursor_x_{0};
  uint16_t cursor_y_{0};
  uint8_t color_{0x0F};

  uint16_t vga_entry(char c) const noexcept;
  void put_at(char c, uint16_t x, uint16_t y) noexcept;

  void scroll_if_needed() noexcept;
  void update_cursor() const noexcept;
  void enable_cursor(uint8_t start, uint8_t end) const noexcept;
};

}  // namespace x86::console
