#pragma once

#include "arch/x86/io/ports.hpp"

namespace x86::input {

class Keyboard {
 public:
  uint8_t read_scancode() const noexcept;

 private:
  io::Port8 data_{0x60};
  io::Port8 status_{0x64};
};

}  // namespace x86::peripheral
