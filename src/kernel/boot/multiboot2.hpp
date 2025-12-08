#pragma once

#include <cstdint>

#include "kernel/containers/flatmap.hpp"

namespace mb2 {

enum class TagType : uint32_t {
  End = 0,

  Cmdline = 1,
  BootloaderName = 2,
  Module = 3,
  BasicMeminfo = 4,
  BootDevice = 5,
  Mmap = 6,
  Vbe = 7,
  Framebuffer = 8,
  ElfSections = 9,
  Apm = 10,

  Efi32SystemTable = 11,
  Efi64SystemTable = 12,

  Smbios = 13,
  AcpiV1RsdPtr = 14,
  AcpiV2RsdPtr = 15,

  Network = 16,
  EfiMemoryMap = 17,
  EfiBootServicesNotTerminated = 18,

  Efi32ImageHandle = 19,
  Efi64ImageHandle = 20,

  ImageLoadBaseAddress = 21

};

constexpr size_t TAG_COUNT = 22;

struct __attribute__((packed)) TagHeader {
  TagType type;
  uint32_t size;
};

constexpr size_t HeaderSize = sizeof(TagHeader);

// Types 1 and 2 (i.e. Cmdline and BootloaderName)
struct __attribute__((packed)) StringOnlyTag {
  char str[0];
};

/// Type 4
/// @brief `mem_lower` and `mem_upper` are in kilobytes. `mem_lower` starts at addr 0 and `mem_upper` at 1 megabyte
struct __attribute__((packed)) BasicMemInfoTag {
  uint32_t mem_lower;
  uint32_t mem_upper;
};

// Type 6
struct __attribute__((packed)) MmapTag {
  uint32_t entry_size;
  uint32_t entry_version;

  struct __attribute__((packed)) Entry {
    uint64_t base_addr;
    uint64_t length;

    enum class Type : uint32_t {
      AvailableRAM = 1,
      ApciInfo = 3,
      Reserved = 4,
      DefectiveRAM = 5,
    } type;

    uint32_t reserved;
  } entries[0];
};

// Type 8
struct __attribute__((packed)) FramebufferTag {
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type_fb;
  uint16_t reserved;
};

struct BasicMemInfoInfo {
  uint32_t mem_lower;
  uint32_t mem_upper;
};

ctr::FlatMap<TagType, TagHeader*, TAG_COUNT>& get_tag_map(
    uint32_t mb2_info_addr = 0x0) noexcept;

bool get_cmdline(const char** cmdline, uint32_t mb2_info_addr = 0x0);
bool get_bootloader_name(const char** bootloader_name, uint32_t mb2_info_addr = 0x0);

template <TagType Type, typename Tag>
bool load_tag(Tag& out, uint32_t mb2_info_addr = 0x0) noexcept {
  if (auto* header = get_tag_map(mb2_info_addr).find(Type)) {
    auto* data =
        reinterpret_cast<Tag*>(reinterpret_cast<uintptr_t>(*header) + HeaderSize);
    out = *data;
    return true;
  }
  return false;
}

template <TagType Type, typename Tag>
bool load_tag(Tag** out, uint32_t mb2_info_addr = 0x0) noexcept {
  if (auto* header = get_tag_map(mb2_info_addr).find(Type)) {
    auto* data =
        reinterpret_cast<Tag*>(reinterpret_cast<uintptr_t>(*header) + HeaderSize);
    *out = data;
    return true;
  }
  *out = nullptr;
  return false;
}

}  // namespace mb2
