#include "arch/x86/boot/multiboot2.hpp"

#include <cstdint>

namespace x86::multiboot2 {

bool find_framebuffer(std::uint32_t mb2_info_addr, FramebufferInfo& out) {

  auto* base = reinterpret_cast<uint8_t*>(static_cast<uintptr_t>(mb2_info_addr));

  Tag* tag = reinterpret_cast<Tag*>(base + 8);

  while (tag->type != TAG_TYPE_END) {
    if (tag->type == TAG_TYPE_FRAMEBUFFER) {
      auto* fb = reinterpret_cast<FramebufferTag*>(tag);

      out.addr = fb->addr;
      out.pitch = fb->pitch;
      out.width = fb->width;
      out.height = fb->height;
      out.bpp = fb->bpp;
      out.type_fb = fb->type_fb;
      return true;
    }

    // Move to next tag (8-byte aligned)
    uint8_t* next = reinterpret_cast<uint8_t*>(tag) + tag->size;
    uintptr_t aligned = (reinterpret_cast<uintptr_t>(next) + 7u) & ~static_cast<uintptr_t>(7u);
    tag = reinterpret_cast<Tag*>(aligned);
  }

  return false;
}

}  // namespace x86::multiboot2
