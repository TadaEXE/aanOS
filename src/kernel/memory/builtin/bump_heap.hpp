#pragma once

#include "kernel/memory/heap.hpp"

namespace mem {

constexpr size_t HEAP_SIZE = 1024 * 1024;

class BumpHeap : public Heap {
 public:
  BumpHeap(const BumpHeap&) = delete;
  BumpHeap(BumpHeap&&) = delete;
  BumpHeap& operator=(const BumpHeap&) = delete;
  BumpHeap& operator=(BumpHeap&&) = delete;

  void init(uintptr_t addr, size_t size = 1 * MiB) noexcept override {
    (void)addr;
    (void)size;
  };

  void* alloc(size_t size, size_t align = alignof(max_align_t)) noexcept override;
  void free(void* ptr) noexcept override;
  void reset() noexcept;
};
}  // namespace mem
