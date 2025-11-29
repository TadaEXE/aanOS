#pragma once

namespace ui {
class IConsole {
public:
  virtual ~IConsole() = default;

  virtual void init() noexcept = 0;
  virtual void clear() noexcept = 0;
  virtual void write_char(char c) noexcept = 0;
  virtual void write_string(const char *s) noexcept = 0;
  virtual void backspace() noexcept = 0;
};

} // namespace hw::console
