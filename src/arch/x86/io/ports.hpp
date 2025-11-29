#pragma once

#include <cstdint>

namespace x86::io {

struct Port8 {
  uint16_t port;

  uint8_t in() const noexcept {
    uint8_t value;
    asm volatile("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
  }

  void out(uint8_t value) const noexcept { asm volatile("outb %0, %1" ::"a"(value), "Nd"(port)); }
};

}  // namespace x86::io
