#include <cstdarg>

#include "hal/serial.hpp"
#include "kernel/logging/log.hpp"

namespace log {

void msg(const char* msg) {
  if (auto* serial = hal::get_serial_bus(hal::SerialPort::SYSTEM_RESERVED)) {
    serial->write_string(msg);
    serial->write_char('\n');
  }
}

}  // namespace log
