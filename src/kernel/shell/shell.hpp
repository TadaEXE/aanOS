#pragma once

#include <cstddef>
#include <string_view>

#include "kernel/containers/string.hpp"
#include "kernel/tty/tty.hpp"

namespace shell {

struct Command;
class Shell;

struct CommandContext {
  Shell& shell;
  tty::Tty& tty;
  size_t argc;
  std::string_view* argv;
};

using CommandFn = void (*)(CommandContext&);

struct Command {
  std::string_view name;
  std::string_view help;
  CommandFn fn;
};

class Shell {
 public:
  // TODO: Make change this shit heap allocated after I add a std::vector knockoff
  static constexpr size_t MAX_COMMANDS = 32;
  static constexpr size_t MAX_ARGS = 32;

  explicit Shell(tty::Tty& tty) noexcept : tty(tty), prompt("/>") {}

  bool register_command(const Command& cmd) noexcept;
  void register_builtin_commands() noexcept;

  [[noreturn]] void run() noexcept;

  void set_prompt(std::string_view prompt) noexcept;

  tty::Tty& get_tty() noexcept { return tty; }

  size_t cmd_count() const noexcept { return cmd_len; }

  const Command& get_cmd(size_t idx) const noexcept { return cmds[idx]; }

 private:
  Command* find_cmd(std::string_view name) noexcept;
  void execute_line(const ctr::String& line) noexcept;

  tty::Tty& tty;
  Command cmds[MAX_COMMANDS];
  size_t cmd_len{0};
  std::string_view prompt;
};
}  // namespace shell
