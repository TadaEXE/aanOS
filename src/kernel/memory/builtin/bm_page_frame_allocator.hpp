#pragma once

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "hal/page_frame_allocator.hpp"

namespace mem::builtin {

class BmPageFrameAllocator final : public hal::PageFrameAllocator {
 public:
  static constexpr uintptr_t PageSize = 4096;

  BmPageFrameAllocator() noexcept = default;

  void init(void* bitmap_storage, size_t bitmap_bytes, uintptr_t managed_base,
            size_t managed_frames) noexcept;

  void add_usable_range(uintptr_t start, uintptr_t end) noexcept;

  uintptr_t alloc_frame() noexcept override;
  void free_frame(uintptr_t addr) noexcept override;
  void reserve_range(uintptr_t start, size_t len) noexcept override;

 private:
  static constexpr uintptr_t align_up(uintptr_t v) noexcept {
    return (v + (PageSize - 1)) & ~(PageSize - 1);
  }

  static constexpr uintptr_t align_down(uintptr_t v) noexcept {
    return v & ~(PageSize - 1);
  }

  bool bit_test(size_t i) const noexcept;
  void bit_set(size_t i) noexcept;
  void bit_clear(size_t i) noexcept;

  void mark_used(uintptr_t start, uintptr_t end) noexcept;
  void mark_free(uintptr_t start, uintptr_t end) noexcept;

  bool in_managed(uintptr_t addr) const noexcept;
  size_t index_of(uintptr_t addr) const noexcept;

  uint8_t* bitmap{nullptr};
  size_t bitmap_bytes{0};

  uintptr_t base{0};
  size_t frames{0};

  size_t hint{0};
};
}  // namespace mem::builtin
