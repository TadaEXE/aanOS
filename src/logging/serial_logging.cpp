#include <cstdarg>
#include "hw/io/serial.hpp"
#include "logging/logging.hpp"

namespace log {

void msg(const char* msg) {
  hw::io::S1().write_string(msg);
}

}  // namespace log
