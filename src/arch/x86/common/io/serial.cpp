#include "arch/x86/common/io/serial.hpp"

#include "hal/serial.hpp"

namespace x86::io {

void SerialBus::init() noexcept {
  // Disable interrupts
  int_en.out(0x00);

  // Enable DLAB (set baud divisor)
  line_ctrl.out(0x80);

  // Set divisor for 38400 baud: divisor = 3
  data.out(0x03);    // low byte
  int_en.out(0x00);  // high byte

  // 8 bits, no parity, one stop bit, clear DLAB
  line_ctrl.out(0x03);

  // Enable FIFO, clear them, 14-byte threshold
  fifo_ctrl.out(0xC7);

  // IRQs enabled, RTS/DSR set
  modem_ctrl.out(0x0B);
}

void SerialBus::write_char(char c) noexcept {
  if (c == '\n') {
    write_raw('\r');
    write_raw('\n');
  } else {
    write_raw(c);
  }
}

void SerialBus::write_string(const char* s) noexcept {
  while (*s != '\0') {
    write_char(*s);
    ++s;
  }
}

void SerialBus::write_hex32(uint32_t v) noexcept {
  static const char* hex = "0123456789ABCDEF";
  write_string("0x");
  for (int8_t i = 7; i >= 0; --i) {
    uint8_t nibble = (v >> (i * 4)) & 0xF;
    write_char(hex[nibble]);
  }
}

void SerialBus::write_raw(char c) noexcept {

  while ((line_status.in() & 0x20) == 0) {}
  data.out(static_cast<uint8_t>(c));
}

}  // namespace x86::io

namespace hal {

SerialBus* SerialBus::get(SerialPort port) noexcept {

  switch (port) {

    case hal::SerialPort::SYSTEM_RESERVED:
      static x86::io::SerialBus serial0(hal::SerialPort::SERIAL_0);
      return &serial0;

    default:
      return nullptr;
  }
}

}  // namespace hal
