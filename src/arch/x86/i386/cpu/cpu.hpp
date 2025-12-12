#pragma once

#include <cstdint>

#include "hal/cpu.hpp"

namespace i386::cpu {

struct Regs32 {
  uint32_t edi;
  uint32_t esi;
  uint32_t ebp;
  uint32_t esp;
  uint32_t ebx;
  uint32_t edx;
  uint32_t ecx;
  uint32_t eax;
  uint32_t eip;
  uint32_t eflags;
  uint32_t cr3;
};

class Context32 final : public hal::CpuContext {
 public:
  Regs32 regs{};
};

class Cpu32 final : public hal::Cpu {
 public:
  bool create_context(hal::CpuContext& context, hal::EntryFn entry, void* arg,
                      uintptr_t stack_top) const noexcept override;
  void switch_to(const hal::CpuContext& next) noexcept override;

  const hal::CpuContext& current_context() noexcept override { return current; }

 private:
  Context32 current;
};
}  // namespace x86::cpu
