#pragma once

#include <cstdint>
#include <cstddef>

namespace hal {

class PageFrameAllocator {
 public:
  virtual ~PageFrameAllocator() = default;

  virtual uintptr_t alloc_frame() noexcept = 0;
  virtual void free_frame(uintptr_t addr) noexcept = 0;

  virtual void reserve_range(uintptr_t start, size_t len) noexcept = 0;
};

}  // namespace hal
