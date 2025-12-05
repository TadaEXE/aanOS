#pragma once

#include <cstddef>
#include <cstdint>

#include "kernel/memory/heap.hpp"

namespace mem::builtin {

struct HeapBlock {
  HeapBlock* next;
  size_t div_count;
  size_t div_size;
  size_t remaining;
};

class BmHeap : public Heap {
 public:
  BmHeap() = default;

  BmHeap(const BmHeap&) = delete;
  BmHeap(BmHeap&&) = delete;
  BmHeap& operator=(const BmHeap&) = default;
  BmHeap& operator=(BmHeap&&) = delete;

  BmHeap(uintptr_t addr, size_t size, size_t default_div_size = 16)
      : default_div_size(default_div_size) {
    init(addr, size);
  }

  bool add_block(uintptr_t addr, size_t size) noexcept;
  bool add_block(uintptr_t addr, size_t size, size_t div_size) noexcept;

  void init(uintptr_t addr, size_t size) noexcept override;
  void* alloc(size_t size, size_t align = alignof(max_align_t)) noexcept override;
  void free(void* ptr) noexcept override;

 private:
  static uint8_t* bitmap_for(HeapBlock* block) {
    return reinterpret_cast<uint8_t*>(block + 1);
  }

  uint8_t* data_begin(HeapBlock* block) { return bitmap_for(block) + block->div_count; }

  uint8_t* data_end(HeapBlock* block) {
    return data_begin(block) + (block->div_count * block->div_size);
  }

  uint8_t next_id() {
    if (last_id == 0xFF) last_id = 0x0;
    return ++last_id;
  }

  HeapBlock* front{nullptr};
  uint8_t last_id{0x0};
  size_t default_div_size{16};
};
}  // namespace mem::builtin
