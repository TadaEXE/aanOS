#pragma once
#include <cstdarg>

#include "kernel/logging/logging.hpp"

namespace logging::backend {

class CallbackSink final : public LoggingSink {
 public:
  CallbackSink(const CallbackSink&) = delete;
  CallbackSink(CallbackSink&&) = delete;
  CallbackSink& operator=(const CallbackSink&) = default;
  CallbackSink& operator=(CallbackSink&&) = default;

  explicit CallbackSink(void (*put_char_cb)(char c, void* ctx), void* ctx)
      : put_char_cb(put_char_cb), ctx(ctx) {}

  void set_cb(void (*cb)(char c, void* ctx), void* ctx) {
    put_char_cb = cb;
    this->ctx = ctx;
  }

  void put_char(char c) noexcept override {
    if (put_char_cb) put_char_cb(c, ctx);
  }

 private:
  void (*put_char_cb)(char c, void* ctx);
  void* ctx;
};

}  // namespace logging::backend
