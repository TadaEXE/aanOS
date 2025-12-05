#include "kernel/shell/shell.hpp"

#include <cstddef>
#include <cstring>
#include <string_view>

#include "hal/system.hpp"
#include "kernel/containers/string.hpp"
#include "kernel/tty/tty.hpp"

namespace shell {
namespace builtin {

int cmd_help(CommandContext& ctx) noexcept {
  ctx.tty.write_line("Available commands:");
  for (size_t i = 0; i < ctx.shell.cmd_count(); ++i) {
    const auto& cmd = ctx.shell.get_cmd(i);

    ctx.tty.write(cmd.name);
    if (!cmd.help.empty()) {
      ctx.tty.write_line(std::string_view{" - "});
      ctx.tty.write(cmd.help);
    }
    ctx.tty.write_line("\n\n");
  }

  return 0;
}

int cmd_echo(CommandContext& ctx) noexcept {
  for (size_t i = 1; i < ctx.argc; ++i) {
    ctx.tty.write(ctx.argv[i]);
    if (i + 1 < ctx.argc) { ctx.tty.write(std::string_view(" ")); }
  }
  ctx.tty.write_char('\n');
  return 0;
}

static constexpr std::string_view SYS_OPT_REBOOT = "reboot";
static constexpr std::string_view SYS_OPT_SHUTDOWN = "shutdown";

int cmd_sys(CommandContext& ctx) noexcept {
  if (ctx.argc != 2) return 1;

  auto& opt = ctx.argv[1];
  if (strncmp(opt.data(), SYS_OPT_REBOOT.data(), opt.size()) == 0) {
    ctx.tty.write_line("Reboot...");
    hal::sys::reboot();
  } else if (strncmp(opt.data(), SYS_OPT_SHUTDOWN.data(), opt.size()) == 0) {
    ctx.tty.write_line("Shutdown...");
    hal::sys::shutdown();
  }

  return 1;
}

}  // namespace builtin

bool Shell::register_command(const Command& cmd) noexcept {
  if (cmd_len >= MAX_COMMANDS) return false;

  cmds[cmd_len++] = cmd;
  return true;
}

[[noreturn]] void Shell::run() noexcept {
  ctr::String line;
  for (;;) {
    tty.readline(line, prompt);
    execute_line(line);
  }
}

void Shell::register_builtin_commands() noexcept {
  Command help_cmd{
      .name = "help",
      .help = "List available commands",
      .fn = &builtin::cmd_help,
  };
  register_command(help_cmd);

  Command echo_cmd{
      .name = "echo",
      .help = "Echo arguments",
      .fn = &builtin::cmd_echo,
  };
  register_command(echo_cmd);

  Command sys_cmd{
      .name = "sys",
      .help =
          "Control system \n"
          "sys [COMMAND]\n"
          "Commands:\n"
          "    reboot\n"
          "    shutdown",
      .fn = &builtin::cmd_sys,
  };
  register_command(sys_cmd);
}

void Shell::set_prompt(std::string_view prompt) noexcept {
  this->prompt = prompt;
}

Command* Shell::find_cmd(std::string_view name) noexcept {
  if (name.size() == 0) return nullptr;

  for (size_t i = 0; i < cmd_len; ++i) {
    if (cmds[i].name == name) { return &cmds[i]; }
  }

  return nullptr;
}

void Shell::execute_line(const ctr::String& line) noexcept {
  const char* data = line.data();
  const size_t len = line.size();

  if (!data || len == 0) return;

  std::string_view argv_buf[MAX_ARGS];
  size_t argc = 0;

  size_t i = 0;
  while (i < len && argc < MAX_ARGS) {
    while (i < len && (data[i] == ' ' || data[i] == '\t')) {
      ++i;
    }

    if (i >= len) break;

    size_t start = i;
    while (i < len && data[i] != ' ' && data[i] != '\t') {
      ++i;
    }

    argv_buf[argc++] = std::string_view{data + start, i - start};
  }

  if (argc == 0) return;

  auto* cmd = find_cmd(argv_buf[0]);
  if (!cmd) {
    tty.write(std::string_view{"Unknown command: "});
    tty.write_line(argv_buf[0]);
    return;
  }

  CommandContext ctx{
      .shell = *this,
      .tty = tty,
      .argc = argc,
      .argv = argv_buf,
  };

  cmd->fn(ctx);
}

}  // namespace shell
