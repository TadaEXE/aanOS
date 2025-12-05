#pragma once

#include "arch/x86/io/ports.hpp"
#include "hal/keyboard.hpp"

namespace x86::input {

enum class RawEventType : uint8_t {
  Press,
  Release,
};

struct RawKeyEvent {
  uint8_t scan_code;
  bool extended;
  RawEventType type;
};

class PS2Keyboard : public hal::Keyboard {
 public:
  static PS2Keyboard& get_instance() {
    static PS2Keyboard instance;
    return instance;
  }

  PS2Keyboard(PS2Keyboard&&) = delete;
  PS2Keyboard& operator=(PS2Keyboard&&) = delete;
  PS2Keyboard(const PS2Keyboard&) = delete;
  void operator=(const PS2Keyboard&) = delete;

  bool poll(hal::KeyEvent& ev) noexcept override;

 private:
  PS2Keyboard() = default;

  bool poll_raw(RawKeyEvent& ev) noexcept;  // override;
  io::Port8 data_port{0x60};
  io::Port8 status_port{0x64};
  bool extended_prefix{false};
};

}  // namespace x86::input
