#pragma once

#include <cstdint>

namespace x86::multiboot2 {

struct Tag {
  uint32_t type;
  uint32_t size;
};

inline constexpr uint32_t TAG_TYPE_END = 0;
inline constexpr uint32_t TAG_TYPE_FRAMEBUFFER = 8;

struct __attribute__((packed)) FramebufferTag {
  uint32_t type;
  uint32_t size;
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type_fb;
  uint16_t reserved;
};

struct FramebufferInfo {
  uint64_t addr;
  uint32_t pitch;
  uint32_t width;
  uint32_t height;
  uint8_t bpp;
  uint8_t type_fb;
};

bool find_framebuffer(uint32_t mb2_info_addr, FramebufferInfo& out);

}  // namespace x86::multiboot2
