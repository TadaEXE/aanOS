#pragma once
#include <cstdint>

#include "kernel/logging/logging.hpp"

namespace hal {

// This enum is brought to you by the medium mighty ChatGPT
enum class Key : uint16_t {
  Unknown = 0x0000,

  Esc = 0x0001,
  Digit1 = 0x0002,
  Digit2 = 0x0003,
  Digit3 = 0x0004,
  Digit4 = 0x0005,
  Digit5 = 0x0006,
  Digit6 = 0x0007,
  Digit7 = 0x0008,
  Digit8 = 0x0009,
  Digit9 = 0x000A,
  Digit0 = 0x000B,
  Minus = 0x000C,
  Equal = 0x000D,
  Backspace = 0x000E,
  Tab = 0x000F,

  Q = 0x0010,
  W = 0x0011,
  E = 0x0012,
  R = 0x0013,
  T = 0x0014,
  Y = 0x0015,
  U = 0x0016,
  I = 0x0017,
  O = 0x0018,
  P = 0x0019,
  LeftBracket = 0x001A,
  RightBracket = 0x001B,
  Enter = 0x001C,
  LeftCtrl = 0x001D,

  A = 0x001E,
  S = 0x001F,
  D = 0x0020,
  F = 0x0021,
  G = 0x0022,
  H = 0x0023,
  J = 0x0024,
  K = 0x0025,
  L = 0x0026,
  Semicolon = 0x0027,
  Apostrophe = 0x0028,
  Grave = 0x0029,

  LeftShift = 0x002A,
  Backslash = 0x002B,
  Z = 0x002C,
  X = 0x002D,
  C = 0x002E,
  V = 0x002F,
  B = 0x0030,
  N = 0x0031,
  M = 0x0032,
  Comma = 0x0033,
  Period = 0x0034,
  Slash = 0x0035,
  RightShift = 0x0036,
  KeypadMultiply = 0x0037,
  LeftAlt = 0x0038,
  Space = 0x0039,

  CapsLock = 0x003A,

  F1 = 0x003B,
  F2 = 0x003C,
  F3 = 0x003D,
  F4 = 0x003E,
  F5 = 0x003F,
  F6 = 0x0040,
  F7 = 0x0041,
  F8 = 0x0042,
  F9 = 0x0043,
  F10 = 0x0044,

  NumLock = 0x0045,
  ScrollLock = 0x0046,

  Keypad7 = 0x0047,
  Keypad8 = 0x0048,
  Keypad9 = 0x0049,
  KeypadMinus = 0x004A,
  Keypad4 = 0x004B,
  Keypad5 = 0x004C,
  Keypad6 = 0x004D,
  KeypadPlus = 0x004E,
  Keypad1 = 0x004F,
  Keypad2 = 0x0050,
  Keypad3 = 0x0051,
  Keypad0 = 0x0052,
  KeypadDot = 0x0053,

  F11 = 0x0057,
  F12 = 0x0058,

  // Extended keys encode 0xE0 in the high byte
  KeypadEnter = 0xE01C,
  RightCtrl = 0xE01D,
  KeypadDivide = 0xE035,
  PrintScreen = 0xE037,
  RightAlt = 0xE038,

  Home = 0xE047,
  Up = 0xE048,
  PageUp = 0xE049,
  Left = 0xE04B,
  Right = 0xE04D,
  End = 0xE04F,
  Down = 0xE050,
  PageDown = 0xE051,
  Insert = 0xE052,
  Delete = 0xE053,

  LeftGui = 0xE05B,
  RightGui = 0xE05C,
  Menu = 0xE05D,
};

enum class KeyEventType : uint8_t {
  Press,
  Release,
};

enum class KeyMod : uint16_t {
  None = 0,
  Shift = 1u << 0,
  Ctrl = 1u << 1,
  Alt = 1u << 2,
  AltGr = 1u << 3,
  Super = 1u << 4,
  CapsLock = 1u << 5,
  NumLock = 1u << 6,
  ScrollLock = 1u << 7,
};

inline KeyMod operator|(KeyMod a, KeyMod b) noexcept {
  return static_cast<KeyMod>(static_cast<uint16_t>(a) | static_cast<uint16_t>(b));
}

inline KeyMod operator&(KeyMod a, KeyMod b) noexcept {
  return static_cast<KeyMod>(static_cast<uint16_t>(a) & static_cast<uint16_t>(b));
}

inline KeyMod operator^(KeyMod a, KeyMod b) noexcept {
  return static_cast<KeyMod>(static_cast<uint16_t>(a) ^ static_cast<uint16_t>(b));
}

inline KeyMod& operator|=(KeyMod& a, KeyMod b) noexcept {
  a = a | b;
  return a;
}

inline KeyMod& operator&=(KeyMod& a, KeyMod b) noexcept {
  a = a & b;
  return a;
}

inline KeyMod operator~(KeyMod a) noexcept {
  return static_cast<KeyMod>(~static_cast<uint16_t>(a));
}

inline bool any(KeyMod m) noexcept {
  return static_cast<uint16_t>(m) != 0;
}

constexpr KeyMod modifier_for_key(const Key key) noexcept {
  switch (key) {
    case Key::LeftShift:
      [[fallthrough]];
    case Key::RightShift:
      return KeyMod::Shift;

    case Key::LeftCtrl:
      [[fallthrough]];
    case Key::RightCtrl:
      return KeyMod::Ctrl;

    case Key::LeftAlt:
      return KeyMod::Alt;

    case Key::RightAlt:
      return KeyMod::AltGr;

    case Key::LeftGui:
      [[fallthrough]];
    case Key::RightGui:
      return KeyMod::Super;

    case Key::CapsLock:
      return KeyMod::CapsLock;

    case Key::NumLock:
      return KeyMod::NumLock;

    case Key::ScrollLock:
      return KeyMod::ScrollLock;

    default:
      return KeyMod::None;
  }
}

constexpr bool is_lock_key(const Key key) noexcept {
  return key == Key::CapsLock || key == Key::NumLock || key == Key::ScrollLock;
}

struct KeyEvent : logging::Loggable {
  Key key;
  KeyEventType type;
  KeyMod mods;
  uint8_t scan_code;
  bool extended;

  constexpr KeyEvent() = default;

  constexpr KeyEvent(Key key, KeyEventType type, KeyMod mods, uint8_t scan_code,
                     bool extended)
      : key(key), type(type), mods(mods), scan_code(scan_code), extended(extended) {}

  static constexpr const char* fmt() noexcept {
    return "{key: %x, type: %u, mods: %x, scan_code: %x, extended: %u}";
  }

  inline bool has_mod(KeyMod mod) const { return (mods & mod) == mod; }

  inline bool has_only_mod(KeyMod mod) const { return (mods ^ mod) == KeyMod::None; }

  void log_self() const noexcept override {
    log_obj<KeyEvent>(key, type, mods, scan_code, extended);
  }
};

class Keyboard {
 public:
  virtual ~Keyboard() = default;
  virtual bool poll(KeyEvent& ev) noexcept = 0;
  static Keyboard& get() noexcept;
};

}  // namespace hal
