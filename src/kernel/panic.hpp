#pragma once

#include <source_location>
#include <utility>

#include "hal/system.hpp"
#include "kernel/log.hpp"

template <typename... Args>
struct panic {
  [[noreturn]] panic(
      const char* fmt, Args&&... args,
      const std::source_location location = std::source_location::current()) {
    log_msg("\n[KERNEL PANIC] \\");
    log_msg(fmt, std::forward<Args>(args)...);
    log_msg("at %s:%d in `%s`", location.file_name(), location.line(),
            location.function_name());

    hal::sys::halt();
  }
};

template <typename... Args>
panic(const char*, Args&&...) -> panic<Args...>;
