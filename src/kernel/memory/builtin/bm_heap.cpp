#include "kernel/memory/builtin/bm_heap.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

#include "kernel/panic.hpp"
#include "math/bit_logic.hpp"

namespace mem::builtin {

bool BmHeap::add_block(uintptr_t addr, size_t size) noexcept {
  return add_block(addr, size, default_div_size);
}

bool BmHeap::add_block(uintptr_t addr, size_t size, size_t div_size) noexcept {
  if (!addr || div_size == 0) return false;
  if (size <= sizeof(HeapBlock) + div_size) return false;

  auto* block = reinterpret_cast<HeapBlock*>(addr);

  size_t space = size - sizeof(HeapBlock);
  size_t max_divs = space / (div_size + 1);

  if (max_divs == 0) return false;

  constexpr size_t DATA_ALIGN = alignof(max_align_t);
  uintptr_t bitmap_base = reinterpret_cast<uintptr_t>(block + 1);
  uintptr_t max_data_addr = bitmap_base + max_divs;
  uintptr_t aligned_data_addr = align_to(max_data_addr, DATA_ALIGN);

  if (aligned_data_addr <= bitmap_base) return false;

  size_t div_count = static_cast<size_t>(aligned_data_addr - bitmap_base);

  block->div_count = div_count;
  block->div_size = div_size;
  block->remaining = div_count * div_size;

  auto* bitmap = bitmap_for(block);
  memset(bitmap, 0, div_count);

  block->next = front;
  front = block;
  return true;
}

void BmHeap::init(uintptr_t addr, size_t size) noexcept {
  if (!add_block(addr, size)) { panic("Heap initialization failed"); }
}

void* BmHeap::alloc(size_t size, size_t align) noexcept {
  if (!size) return nullptr;

  if (align == 0) { align = alignof(max_align_t); }

  if (!bits::ipo2(align)) {
    panic("Tried to allocate missaligned memory (Not a power of 2).");
  }

  for (HeapBlock* block = front; block != nullptr; block = block->next) {
    if (block->remaining < size) continue;

    size_t needed_divs = bits::oiz((size + block->div_size - 1) / block->div_size);
    auto* bitmap = bitmap_for(block);
    auto* data = data_begin(block);
    uintptr_t data_addr = reinterpret_cast<uintptr_t>(data);

    size_t run_len = 0;
    size_t run_start = 0;

    for (size_t i = 0; i < block->div_count; ++i) {
      if (bitmap[i] != 0) {
        run_len = 0;
        continue;
      }

      if (run_len == 0) {
        uintptr_t candidate = data_addr + i * block->div_size;
        if ((candidate % align) != 0) { continue; }
        run_start = i;
        run_len = 1;
      } else {
        ++run_len;
      }

      if (run_len >= needed_divs) {
        uintptr_t base_addr = data_addr + run_start * block->div_size;

        auto id = next_id();
        memset(bitmap + run_start, id, needed_divs);

        block->remaining -= needed_divs * block->div_size;
        return reinterpret_cast<void*>(base_addr);
      }
    }
  }
  return nullptr;
}

void BmHeap::free(void* ptr) noexcept {
  if (!ptr) return;

  auto* p = reinterpret_cast<uint8_t*>(ptr);

  for (HeapBlock* block = front; block != nullptr; block = block->next) {
    auto* begin = data_begin(block);
    auto* end = data_end(block);

    if (p < begin || p >= end) continue;

    size_t offset = static_cast<size_t>(p - begin);
    size_t idx = offset / block->div_size;

    auto* bitmap = bitmap_for(block);
    auto id = bitmap[idx];
    if (id == 0) { panic("Double free"); }

    size_t freed_divs = 0;
    for (size_t i = idx; i < block->div_count && bitmap[i] == id; ++i) {
      bitmap[i] = 0;
      ++freed_divs;
    }

    block->remaining += freed_divs * block->div_size;
    return;
  }
}

}  // namespace mem::builtin
