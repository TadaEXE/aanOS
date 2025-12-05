#include "kernel/memory/builtin/bump_heap.hpp"

#include <cstddef>
#include <cstdint>

#include "kernel/memory/heap.hpp"

namespace mem {

namespace {

alignas(16) uint8_t heap_buffer[HEAP_SIZE];
size_t heap_offset = 0;

}  // namespace

void* BumpHeap::alloc(size_t size, size_t align) noexcept {
  if (align < alignof(max_align_t)) { align = alignof(max_align_t); }

  const uintptr_t base = reinterpret_cast<uintptr_t>(heap_buffer);
  const uintptr_t current = base + heap_offset;

  const uintptr_t aligned = align_to(current, align);
  const uintptr_t new_end = aligned + size;

  if (new_end > base + HEAP_SIZE) {
    // Out of memory – we'll handle this in global new below.
    return nullptr;
  }

  heap_offset = static_cast<size_t>(new_end - base);
  return reinterpret_cast<void*>(aligned);
}

void BumpHeap::free(void* /*ptr*/) noexcept {
  // No-op for bump allocator.
  // You could add debug bookkeeping here if you want.
}

void BumpHeap::reset() noexcept {
  heap_offset = 0;
}

}  // namespace mem
