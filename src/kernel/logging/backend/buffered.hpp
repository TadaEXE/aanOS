#pragma once
#include <cstdarg>
#include <cstring>

#include "kernel/logging/logging.hpp"

namespace logging::backend {

/// Do not use! Performs like shit.
template <size_t MaxSubCount>
class BufferedSink final : public LoggingSink {
 public:
  BufferedSink() = default;

  BufferedSink(const BufferedSink&) = delete;
  BufferedSink(BufferedSink&&) = delete;
  BufferedSink& operator=(const BufferedSink&) = delete;
  BufferedSink& operator=(BufferedSink&&) = delete;

  bool add_sub(LoggingSink* sub) {
    if (sub_count >= MaxSubCount) return false;

    subs[sub_count++] = sub;
    flush_to_new_sub(sub);
    return true;
  }

  void reflush() {
    for (auto* s : subs) {
      flush_to_new_sub(s);
    }
  }

  void put_char(char c) noexcept override {
    if (buffer_len >= max_buffer_len) grow();

    buffer[buffer_len++] = c;
    forward_to_subs(c);
  }

 private:
  void forward_to_subs(char c) {
    for (size_t i = 0; i < sub_count; ++i) {
      subs[i]->put_char(c);
    }
  }

  void flush_to_new_sub(LoggingSink* sub) {
    for (size_t i = 0; i < buffer_len; ++i) {
      sub->put_char(buffer[i]);
    }
  }

  void grow() {
    memmove(buffer, buffer + (max_buffer_len - buffer_end_chunk), buffer_end_chunk);
    buffer_len = buffer_end_chunk;
    buffer_len = 0;
  }

  LoggingSink* subs[MaxSubCount];
  size_t sub_count{0};

  char buffer[1024];
  size_t buffer_len{0};
  size_t max_buffer_len{1024};
  size_t buffer_end_chunk{64};
};

}  // namespace logging::backend
