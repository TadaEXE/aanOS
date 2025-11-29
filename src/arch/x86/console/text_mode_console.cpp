#include "arch/x86/console/text_mode_console.hpp"

#include "arch/x86/io/ports.hpp"

namespace x86::console {

namespace {
// VGA text buffer at physical address 0xB8000
volatile uint16_t* const VGA_MEMORY = reinterpret_cast<volatile uint16_t*>(0xB8000);
}  // namespace

void TextModeConsole::init() noexcept {
  cursor_x_ = 0;
  cursor_y_ = 0;
  clear();
  enable_cursor(0, 15);  // full-height blinking block
  update_cursor();
}

void TextModeConsole::clear() noexcept {
  for (uint16_t y = 0; y < 25; ++y) {
    for (uint16_t x = 0; x < 80; ++x) {
      put_at(' ', x, y);
    }
  }
  cursor_x_ = 0;
  cursor_y_ = 0;
  update_cursor();
}

uint16_t TextModeConsole::vga_entry(char c) const noexcept {
  return static_cast<uint16_t>(static_cast<uint8_t>(c)) | (static_cast<uint16_t>(color_) << 8u);
}

void TextModeConsole::put_at(char c, uint16_t x, uint16_t y) noexcept {
  VGA_MEMORY[y * 80 + x] = vga_entry(c);
}

void TextModeConsole::scroll_if_needed() noexcept {
  if (cursor_y_ < 25) {
    return;
  }

  // Move lines 1..24 -> 0..23
  for (uint16_t y = 1; y < 25; ++y) {
    for (uint16_t x = 0; x < 80; ++x) {
      VGA_MEMORY[(y - 1) * 80 + x] = VGA_MEMORY[y * 80 + x];
    }
  }

  // Clear last line
  for (uint16_t x = 0; x < 80; ++x) {
    VGA_MEMORY[(25 - 1) * 80 + x] = vga_entry(' ');
  }

  cursor_y_ = 25 - 1;
}

void TextModeConsole::update_cursor() const noexcept {
  const uint16_t pos = cursor_y_ * 80 + cursor_x_;

  io::Port8 index{0x3D4};
  io::Port8 data{0x3D5};

  index.out(0x0F);
  data.out(static_cast<uint8_t>(pos & 0xFFu));
  index.out(0x0E);
  data.out(static_cast<uint8_t>((pos >> 8) & 0xFFu));
}

void TextModeConsole::enable_cursor(uint8_t start, uint8_t end) const noexcept {
  io::Port8 index{0x3D4};
  io::Port8 data{0x3D5};

  // Cursor start
  index.out(0x0A);
  uint8_t cur_start = data.in();
  data.out((cur_start & 0xC0u) | start);

  // Cursor end
  index.out(0x0B);
  uint8_t cur_end = data.in();
  data.out((cur_end & 0xE0u) | end);
}

void TextModeConsole::write_char(char c) noexcept {
  if (c == '\n') {
    cursor_x_ = 0;
    ++cursor_y_;
    scroll_if_needed();
    update_cursor();
    return;
  }

  if (c == '\t') {
    for (int i = 0; i < 4; ++i) {
      write_char(' ');
    }
    return;
  }

  if (cursor_x_ >= 80) {
    cursor_x_ = 0;
    ++cursor_y_;
    scroll_if_needed();
  }

  put_at(c, cursor_x_, cursor_y_);
  ++cursor_x_;
  update_cursor();
}

void TextModeConsole::write_string(const char* s) noexcept {
  uint32_t i = 0;
  while (char c = *(s + i)) {
    write_char(c);
  }
}

void TextModeConsole::backspace() noexcept {
  if (cursor_x_ == 0) {
    return;
  }
  --cursor_x_;
  put_at(' ', cursor_x_, cursor_y_);
  update_cursor();
}

}  // namespace x86::console
