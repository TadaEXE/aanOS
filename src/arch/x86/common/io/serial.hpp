#pragma once
#include <cstdint>

#include "arch/x86/common/io/ports.hpp"
#include "hal/serial.hpp"

namespace x86::io {

constexpr uint16_t S0_BASE = 0x3F8;

constexpr uint16_t PORT_TO_BASE(hal::SerialPort port) {
  switch (port) {
    case hal::SerialPort::SYSTEM_RESERVED:
      return S0_BASE;

    default:
      return 0x0;
  }
}

class SerialBus final : public hal::SerialBus {
 public:
  SerialBus(const SerialBus&) = delete;
  SerialBus(SerialBus&&) = delete;
  SerialBus& operator=(const SerialBus&) = delete;
  SerialBus& operator=(SerialBus&&) = delete;

  SerialBus(hal::SerialPort port)
      : com_base(PORT_TO_BASE(port)),
        data(com_base + 0),
        int_en(com_base + 1),
        fifo_ctrl(com_base + 2),
        line_ctrl(com_base + 3),
        modem_ctrl(com_base + 4),
        line_status(com_base + 5) {}

  void init() noexcept;
  void write_char(char c) noexcept;
  void write_string(const char* s) noexcept;
  void write_hex32(uint32_t v) noexcept;

 private:
  void write_raw(char c) noexcept;

  const uint16_t com_base;

  Port8 data;
  Port8 int_en;
  Port8 fifo_ctrl;
  Port8 line_ctrl;
  Port8 modem_ctrl;
  Port8 line_status;
};

SerialBus& get_serial0() noexcept;

}  // namespace x86::io
