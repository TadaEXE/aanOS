#include "arch/x86/common/input/keyboard.hpp"

#include "hal/keyboard.hpp"

using namespace hal;

namespace x86::input {

bool PS2Keyboard::poll_raw(RawKeyEvent& rke) noexcept {

  uint8_t status = status_port.in();
  if ((status & 0x01u) == 0u) { return false; }

  uint8_t sc = data_port.in();

  if (sc == 0xE0u) {
    extended_prefix = true;
    return false;
  }

  if (sc == 0xE1u) {
    extended_prefix = false;
    return false;
  }

  bool extended = extended_prefix;
  extended_prefix = false;

  RawEventType type;
  uint8_t code;

  if (sc & 0x80u) {
    type = RawEventType::Release;
    code = static_cast<uint8_t>(sc & 0x7Fu);
  } else {
    type = RawEventType::Press;
    code = sc;
  }

  rke.scan_code = code;
  rke.extended = extended;
  rke.type = type;

  return true;
}

bool PS2Keyboard::poll(hal::KeyEvent& ev) noexcept {
  static hal::KeyMod mods = hal::KeyMod::None;

  RawKeyEvent raw{};
  if (!poll_raw(raw)) { return false; }

  uint16_t combined = raw.extended ? static_cast<uint16_t>((0xE0u << 8) | raw.scan_code)
                                   : static_cast<uint16_t>(raw.scan_code);

  hal::Key key = static_cast<hal::Key>(combined);

  hal::KeyEventType type = raw.type == RawEventType::Press ? hal::KeyEventType::Press
                                                           : hal::KeyEventType::Release;

  hal::KeyMod bit = hal::modifier_for_key(key);

  if (bit != hal::KeyMod::None) {
    if (hal::is_lock_key(key)) {
      if (type == hal::KeyEventType::Press) {
        mods = static_cast<hal::KeyMod>(static_cast<uint16_t>(mods) ^
                                        static_cast<uint16_t>(bit));
      }
    } else {
      if (type == hal::KeyEventType::Press) {
        mods |= bit;
      } else {
        mods = static_cast<hal::KeyMod>(static_cast<uint16_t>(mods) &
                                        ~static_cast<uint16_t>(bit));
      }
    }
  }

  ev.key = key;
  ev.type = type;
  ev.mods = mods;
  ev.scan_code = raw.scan_code;
  ev.extended = raw.extended;

  return true;
}
}  // namespace x86::input

namespace hal {
Keyboard& keyboard() noexcept {
  return x86::input::PS2Keyboard::get_instance();
}
}  // namespace hal
