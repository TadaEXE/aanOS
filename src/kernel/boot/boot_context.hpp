#pragma once
#include <cstddef>
#include <cstdint>

#include "hal/framebuffer.hpp"
#include "hal/serial.hpp"

namespace boot {

enum class ArchKind : uint8_t {
  Unknown = 0,
  X86,
  X86_64,
};

constexpr const char* ArchName(ArchKind arch) {
  switch (arch) {
    case ArchKind::Unknown:
      return "Unknown";
    case ArchKind::X86:
      return "X86";
    case ArchKind::X86_64:
      return "X86_64";
  }
}

enum class Bootflags : uint8_t {
  None = 0b00000000,
  NoGUI = 0b00000001,
};

enum class BootLoaderKind : uint8_t {
  Unknown = 0,
  Multiboot2,
  Limine,
  Stivale2,
  UefiDirect,
};

enum class MemoryRegionType : uint8_t {
  Unknown = 0,
  Usable,
  Reserved,
  AcpiReclaimable,
  AcpiNvs,
  Mmio,
  Kernel,
  BootloaderReclaimable,
  Bad,
};

static constexpr const char* MemoryRegionTypeName(MemoryRegionType type) {
  switch (type) {
    case MemoryRegionType::Unknown:
      return "Unknown";
    case MemoryRegionType::Usable:
      return "Usable";
    case MemoryRegionType::Reserved:
      return "Reserved";
    case MemoryRegionType::AcpiReclaimable:
      return "AcpiReclaimable";
    case MemoryRegionType::AcpiNvs:
      return "AcpiNvs";
    case MemoryRegionType::Mmio:
      return "Mmio";
    case MemoryRegionType::Kernel:
      return "Kernel";
    case MemoryRegionType::BootloaderReclaimable:
      return "BootloaderReclaimable";
    case MemoryRegionType::Bad:
      return "Bad";
    default:
      return "Unknown";
  }
}

struct MemoryRegion {
  MemoryRegionType type;
  uintptr_t addr;
  size_t length;
};

struct InitModule {
  const char* name;
  uint64_t phys_base;
  uint64_t length;
};

struct BootModulesView {
  const InitModule* modules;
  size_t count;
};

struct BootContext {
  // Basic identity
  ArchKind arch;
  BootLoaderKind bootloader;
  const char* bootloader_name;
  const char* cmdline;

  // Basic memory
  const uintptr_t upper_mem_start{0x100000};
  size_t upper_mem_kb;

  hal::Framebuffer* boot_framebuffer;
  hal::SerialBus* system_serial_bus;

  // Physical memory layout
  size_t memory_regions = 0;
  // only usable for now
  MemoryRegion* memory_map;

  uintptr_t ram_start_addr;

  // Optional init modules
  // BootModulesView modules;

  // Optional arch-specific extra data
  void* arch_private;
};

}  // namespace boot
