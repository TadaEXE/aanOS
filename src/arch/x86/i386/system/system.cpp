#include "hal/system.hpp"

#include "arch/x86/common/io/ports.hpp"

namespace hal::sys {

[[noreturn]] void halt() noexcept {
  asm volatile("cli");
  for (;;) {
    asm volatile("hlt");
  }
}

[[noreturn]] void reboot() noexcept {
  asm volatile("cli");

  x86::io::Port8 kbd_cmd{0x64};

  for (;;) {
    while (kbd_cmd.in() & 0x02) {}
    kbd_cmd.out(0xFE);
    asm volatile("hlt");
  }
}

[[noreturn]] void shutdown() noexcept {
  asm volatile("cli");

  x86::io::Port16 acpi{0x604};
  acpi.out(0x2000);

  x86::io::Port16 b004{0xB004};
  b004.out(0x2000);

  x86::io::Port16 qemu{0x4004};
  qemu.out(0x3400);

  for (;;) {
    asm volatile("hlt");
  }
}
}  // namespace hal::sys
