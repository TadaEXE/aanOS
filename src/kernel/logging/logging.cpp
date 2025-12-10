#include "logging/logging.hpp"

#include <cstdarg>
#include <cstdint>

namespace logging::internal {
static backend::LoggingSink* log_sink;

void backend_put_char(char c) noexcept {
  if (log_sink) { log_sink->put_char(c); }
}

void backend_put_cstr(const char* s) noexcept {
  if (!s) { return; }
  while (*s) {
    backend_put_char(*s++);
  }
}

void print_uint(uint64_t value, uint32_t base, bool uppercase) noexcept {
  if (base < 2 || base > 16) { return; }

  char buf[32];
  uint32_t i = 0;

  if (value == 0) {
    backend_put_char('0');
    return;
  }

  const char* digits = uppercase ? "0123456789ABCDEF" : "0123456789abcdef";

  while (value > 0 && i < sizeof(buf)) {
    uint32_t digit = static_cast<uint32_t>(value % base);
    buf[i++] = digits[digit];
    value /= base;
  }

  while (i > 0) {
    backend_put_char(buf[--i]);
  }
}

void print_int(int64_t value) noexcept {
  if (value < 0) {
    backend_put_char('-');
    uint64_t mag = static_cast<uint64_t>(-value);
    print_uint(mag, 10, false);
  } else {
    uint64_t mag = static_cast<uint64_t>(value);
    print_uint(mag, 10, false);
  }
}

void vmsg_impl(bool auto_new_line, const char* fmt, va_list args) noexcept {
  if (!fmt) { return; }

  bool skip_new_line = !auto_new_line;
  while (*fmt) {
    char c = *fmt++;

    if (c == '\\') {
      if (*fmt == '\0') {
        skip_new_line = true;
        break;
      } else {
        backend_put_char('\\');
        continue;
      }
    } else if (c != '%') {
      backend_put_char(c);
      continue;
    }

    char spec = *fmt++;
    if (!spec) {
      backend_put_char('%');
      break;
    }

    switch (spec) {
      case '%': {
        backend_put_char('%');
        break;
      }

      case 'c': {
        int v = va_arg(args, int);
        backend_put_char(static_cast<char>(v));
        break;
      }

      case 's': {
        const char* s = va_arg(args, const char*);
        if (!s) {
          backend_put_cstr("<null>");
        } else {
          backend_put_cstr(s);
        }
        break;
      }

      case 'd':
      case 'i': {
        int v = va_arg(args, int);
        print_int(static_cast<int64_t>(v));
        break;
      }

      case 'u': {
        unsigned int v = va_arg(args, unsigned int);
        print_uint(static_cast<uint64_t>(v), 10, false);
        break;
      }

      case 'x': {
        //   unsigned int v = va_arg(args, unsigned int);
        //   backend_put_cstr("0x");
        //   print_uint(static_cast<uint64_t>(v), 16, false);
        //   break;
        // }
        //
        // case 'X': {
        unsigned int v = va_arg(args, unsigned int);
        backend_put_cstr("0x");
        print_uint(static_cast<uint64_t>(v), 16, true);
        break;
      }
      case 'b': {
        unsigned int v = va_arg(args, unsigned int);
        backend_put_cstr("0b");
        print_uint(static_cast<uint64_t>(v), 2, true);
        break;
      }
      case 'p': {
        void* ptr = va_arg(args, void*);
        if (ptr) {
          uintptr_t v = reinterpret_cast<uintptr_t>(ptr);
          backend_put_cstr("0x");
          print_uint(static_cast<uint64_t>(v), 16, false);
        } else {
          backend_put_cstr("<null>");
        }
        break;
      }

      case 'o': {
        auto* obj = va_arg(args, Loggable*);
        if (obj) {
          obj->log_self();
        } else {
          backend_put_cstr("<null>");
        }
        break;
      }

      default: {
        backend_put_char('%');
        backend_put_char(spec);
        break;
      }
    }
  }

  if (!skip_new_line) { backend_put_char('\n'); }
}

}  // namespace logging::internal

namespace logging {

namespace backend {
void set_sink(LoggingSink* sink) {
  internal::log_sink = sink;
}

void log_obj(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  internal::vmsg_impl(false, fmt, args);
  va_end(args);
}
}  // namespace backend
}  // namespace logging

void log_msg(const char* fmt, ...) {
  va_list args;
  va_start(args, fmt);
  logging::internal::vmsg_impl(true, fmt, args);
  va_end(args);
}

void log_msg(const char* fmt, va_list args) {
  logging::internal::vmsg_impl(true, fmt, args);
}
