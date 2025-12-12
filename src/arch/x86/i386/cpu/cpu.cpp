#include "x86/i386/cpu/cpu.hpp"

namespace i386::cpu {

bool Cpu32::create_context(hal::CpuContext& context, hal::EntryFn entry, void* arg,
                           uintptr_t stack_top) const noexcept {
  Context32 ctx32;
  auto& r = ctx32.regs;

  r.edi = 0;
  r.esi = 0;
  r.ebp = static_cast<uint32_t>(stack_top);
  r.ebx = 0;
  r.edx = 0;
  r.ecx = 0;

  r.eax = reinterpret_cast<uint32_t>(arg);
  r.eip = reinterpret_cast<uint32_t>(entry);

  uint32_t eflags = 0;
  asm volatile("pushf\n\tpop %0" : "=r"(eflags));
  r.eflags = eflags;

  r.cr3 = 0;

  context = ctx32;

  return true;
}

void Cpu32::switch_to(const hal::CpuContext& last) noexcept {
  current = *reinterpret_cast<const Context32*>(&last);
}
}  // namespace x86::cpu
