#include "arch/x86/input/keyboard.hpp"

namespace x86::input {

uint8_t Keyboard::read_scancode() const noexcept {
  while ((status_.in() & 0x01u) == 0u) {
    // spin
  }
  return data_.in();
}

}  // namespace x86::input
