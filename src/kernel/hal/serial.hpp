#pragma once

#include <cstdint>

namespace hal {

enum class SerialPort : uint32_t {
  SYSTEM_RESERVED = 0,
  SERIAL_0 = SYSTEM_RESERVED,
};

class SerialBus {
 public:
  virtual ~SerialBus() = default;

  virtual void init() noexcept = 0;
  virtual void write_char(char c) noexcept = 0;
  virtual void write_string(const char* s) noexcept = 0;
  virtual void write_hex32(uint32_t h) noexcept = 0;
  static SerialBus* get(SerialPort port) noexcept;
};

}  // namespace hal
