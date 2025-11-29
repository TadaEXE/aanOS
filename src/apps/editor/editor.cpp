#include "apps/editor/editor.hpp"
#include "ui/core/iconsole.hpp"
#include "hw/peripherals/keyboard.hpp"

#include <cstdint>

namespace input {

namespace {

constexpr char base_map[128] = {
    0,    27,  '1', '2',  '3',  '4', '5',  '6',   // 0x00 - 0x07
    '7',  '8', '9', '0',  '-',  '=', '\b', '\t',  // 0x08 - 0x0F
    'q',  'w', 'e', 'r',  't',  'y', 'u',  'i',   // 0x10 - 0x17
    'o',  'p', '[', ']',  '\n', 0,   'a',  's',   // 0x18 - 0x1F
    'd',  'f', 'g', 'h',  'j',  'k', 'l',  ';',   // 0x20 - 0x27
    '\'', '`', 0,   '\\', 'z',  'x', 'c',  'v',   // 0x28 - 0x2F
    'b',  'n', 'm', ',',  '.',  '/', 0,    '*',   // 0x30 - 0x37
    0,    ' ', 0                                  // 0x38 - ...
                                                  // rest = 0
};

constexpr char shift_map[128] = {
    0,   27,  '!', '@', '#',  '$', '%',  '^',   // 0x00 - 0x07
    '&', '*', '(', ')', '_',  '+', '\b', '\t',  // 0x08 - 0x0F
    'Q', 'W', 'E', 'R', 'T',  'Y', 'U',  'I',   // 0x10 - 0x17
    'O', 'P', '{', '}', '\n', 0,   'A',  'S',   // 0x18 - 0x1F
    'D', 'F', 'G', 'H', 'J',  'K', 'L',  ':',   // 0x20 - 0x27
    '"', '~', 0,   '|', 'Z',  'X', 'C',  'V',   // 0x28 - 0x2F
    'B', 'N', 'M', '<', '>',  '?', 0,    '*',   // 0x30 - 0x37
    0,   ' ', 0                                 // 0x38 - ...
                                                // rest = 0
};

enum class Key : uint8_t {
  Backspace = 0x0E,
  LShift = 0x2A,
  RShift = 0x36,
};

}  // namespace

void Editor::run() noexcept {
  hw::console::TextModeConsole console;
  hw::peripheral::Keyboard keyboard;

  console.init();

  bool shift_down = false;

  constexpr const char* banner = "#editOS#\n";

  for (const char* p = banner; *p != '\0'; ++p) {
    console.write_char(*p);
  }

  for (;;) {
    const uint8_t sc = keyboard.read_scancode();
    const bool rel = (sc & 0x80u) != 0u;
    const uint8_t code = sc & 0x7Fu;

    if (code == static_cast<uint8_t>(Key::LShift) || code == static_cast<uint8_t>(Key::RShift)) {
      shift_down = !rel;
      continue;
    }

    if (code == static_cast<uint8_t>(Key::Backspace) && !rel) {
      console.backspace();
      continue;
    }

    if (rel) {
      continue;
    }

    char c = '\0';
    if (code < 128u) {
      c = shift_down ? shift_map[code] : base_map[code];
    }

    if (c != '\0') {
      console.write_char(c);
    }
  }
}

}  // namespace input
