#include "input/keymap.hpp"

namespace input {

using namespace hal;

struct KeyCharEntry {
  Key key;
  char normal;
  char shifted;
  char altgr;
};

constexpr KeyCharEntry KEYMAP[] = {
    {Key::A, 'a', 'A', 0},
    {Key::B, 'b', 'B', 0},
    {Key::C, 'c', 'C', 0},
    {Key::D, 'd', 'D', 0},
    {Key::E, 'e', 'E', 0},
    {Key::F, 'f', 'F', 0},
    {Key::G, 'g', 'G', 0},
    {Key::H, 'h', 'H', 0},
    {Key::I, 'i', 'I', 0},
    {Key::J, 'j', 'J', 0},
    {Key::K, 'k', 'K', 0},
    {Key::L, 'l', 'L', 0},
    {Key::M, 'm', 'M', 0},
    {Key::N, 'n', 'N', 0},
    {Key::O, 'o', 'O', 0},
    {Key::P, 'p', 'P', 0},
    {Key::Q, 'q', 'Q', 0},
    {Key::R, 'r', 'R', 0},
    {Key::S, 's', 'S', 0},
    {Key::T, 't', 'T', 0},
    {Key::U, 'u', 'U', 0},
    {Key::V, 'v', 'V', 0},
    {Key::W, 'w', 'W', 0},
    {Key::X, 'x', 'X', 0},
    {Key::Y, 'y', 'Y', 0},
    {Key::Z, 'z', 'Z', 0},

    {Key::Digit1, '1', '!', 0},
    {Key::Digit2, '2', '@', 0},
    {Key::Digit3, '3', '#', 0},
    {Key::Digit4, '4', '$', 0},
    {Key::Digit5, '5', '%', 0},
    {Key::Digit6, '6', '^', 0},
    {Key::Digit7, '7', '&', 0},
    {Key::Digit8, '8', '*', 0},
    {Key::Digit9, '9', '(', 0},
    {Key::Digit0, '0', ')', 0},

    {Key::Minus, '-', '_', 0},
    {Key::Equal, '=', '+', 0},
    {Key::LeftBracket, '[', '{', 0},
    {Key::RightBracket, ']', '}', 0},
    {Key::Backslash, '\\', '|', 0},
    {Key::Semicolon, ';', ':', 0},
    {Key::Apostrophe, '\'', '"', 0},
    {Key::Grave, '`', '~', 0},
    {Key::Comma, ',', '<', 0},
    {Key::Period, '.', '>', 0},
    {Key::Slash, '/', '?', 0},
    {Key::Space, ' ', ' ', 0},
    {Key::Enter, '\n', '\n', 0},

    {Key::Keypad0, '0', '0', 0},
    {Key::Keypad1, '1', '1', 0},
    {Key::Keypad2, '2', '2', 0},
    {Key::Keypad3, '3', '3', 0},
    {Key::Keypad4, '4', '4', 0},
    {Key::Keypad5, '5', '5', 0},
    {Key::Keypad6, '6', '6', 0},
    {Key::Keypad7, '7', '7', 0},
    {Key::Keypad8, '8', '8', 0},
    {Key::Keypad9, '9', '9', 0},
    {Key::KeypadPlus, '+', '+', 0},
    {Key::KeypadMinus, '-', '-', 0},
    {Key::KeypadMultiply, '*', '*', 0},
    {Key::KeypadDivide, '/', '/', 0},
    {Key::KeypadDot, '.', '.', 0},
};

constexpr bool is_letter(char c) noexcept {
  return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

bool key_event_to_char(const KeyEvent& ev, char& out) noexcept {
  if (ev.type != KeyEventType::Press) { return false; }

  if ((ev.mods & KeyMod::Ctrl) != KeyMod::None) { return false; }

  if ((ev.mods & KeyMod::Alt) != KeyMod::None) { return false; }

  bool shift = (ev.mods & KeyMod::Shift) != KeyMod::None;
  bool caps = (ev.mods & KeyMod::CapsLock) != KeyMod::None;
  bool altgr = (ev.mods & KeyMod::AltGr) != KeyMod::None;

  for (const auto& e : KEYMAP) {
    if (e.key != ev.key) { continue; }

    char ch = 0;

    if (altgr && e.altgr != 0) {
      ch = e.altgr;
    } else if (is_letter(e.normal)) {
      bool upper = shift ^ caps;
      if (upper && e.shifted != 0) {
        ch = e.shifted;
      } else {
        ch = e.normal;
      }
    } else {
      if (shift && e.shifted != 0) {
        ch = e.shifted;
      } else {
        ch = e.normal;
      }
    }

    if (ch == 0) { return false; }

    out = ch;
    return true;
  }

  return false;
}

}  // namespace input
