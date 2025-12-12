#include "memory/builtin/bm_page_frame_allocator.hpp"

#include <cstddef>
#include <cstdint>
#include <cstring>

namespace mem::builtin {

void BmPageFrameAllocator::init(void* bitmap_storage, size_t bitmap_bytes,
                                uintptr_t managed_base, size_t managed_frames) noexcept {
  bitmap = static_cast<uint8_t*>(bitmap_storage);
  this->bitmap_bytes = bitmap_bytes;

  base = align_up(managed_base);
  frames = managed_frames;

  memset(bitmap, 0xFF, bitmap_bytes);
  hint = 0;

  auto bm_phys = static_cast<uintptr_t>(reinterpret_cast<uintptr_t>(bitmap));
  reserve_range(bm_phys, bitmap_bytes);
}

bool BmPageFrameAllocator::bit_test(size_t i) const noexcept {
  size_t byte = i / 8;
  uint8_t mask = static_cast<uint8_t>(1u << (i % 8));
  if (byte >= bitmap_bytes) return true;
  return (bitmap[byte] & mask) != 0;
}

void BmPageFrameAllocator::bit_set(size_t i) noexcept {
  size_t byte = i / 8;
  uint8_t mask = static_cast<uint8_t>(1u << (i % 8));
  if (byte >= bitmap_bytes) return;
  bitmap[byte] |= mask;
}

void BmPageFrameAllocator::bit_clear(size_t i) noexcept {
  size_t byte = i / 8;
  uint8_t mask = static_cast<uint8_t>(1u << (i % 8));
  if (byte >= bitmap_bytes) return;
  bitmap[byte] &= ~mask;
}

size_t BmPageFrameAllocator::index_of(uintptr_t addr) const noexcept {
  return static_cast<size_t>((addr - base) / PageSize);
}

bool BmPageFrameAllocator::in_managed(uintptr_t addr) const noexcept {
  if ((addr & (PageSize - 1)) != 0) return false;
  if (addr < base) return false;
  auto end = base + static_cast<uintptr_t>(frames * PageSize);
  return addr < end;
}

void BmPageFrameAllocator::mark_used(uintptr_t start, uintptr_t end) noexcept {
  start = align_down(start);
  end = align_up(end);
  if (end <= start) return;

  auto managed_end = base + static_cast<uintptr_t>(frames * PageSize);
  if (start < base) start = base;
  if (end > managed_end) end = managed_end;
  if (end <= start) return;

  auto first = static_cast<size_t>((start - base) / PageSize);
  auto last = static_cast<size_t>((end - base + PageSize - 1) / PageSize);
  if (last > frames) last = frames;

  for (size_t i = first; i < last; ++i)
    bit_set(i);
}

void BmPageFrameAllocator::mark_free(uintptr_t start, uintptr_t end) noexcept {
  start = align_down(start);
  end = align_up(end);
  if (end <= start) return;

  auto managed_end = base + static_cast<uintptr_t>(frames * PageSize);
  if (start < base) start = base;
  if (end > managed_end) end = managed_end;
  if (end <= start) return;

  auto first = static_cast<size_t>((start - base) / PageSize);
  auto last = static_cast<size_t>((end - base + PageSize - 1) / PageSize);
  if (last > frames) last = frames;

  for (size_t i = first; i < last; ++i) {
    bit_clear(i);
  }

  if (first < hint) hint = first;
}

void BmPageFrameAllocator::add_usable_range(uintptr_t start, uintptr_t end) noexcept {
  mark_free(start, end);
}

void BmPageFrameAllocator::reserve_range(uintptr_t start, size_t len) noexcept {
  if (len == 0) return;
  mark_used(start, start + static_cast<uintptr_t>(len));
}

uintptr_t BmPageFrameAllocator::alloc_frame() noexcept {
  for (size_t i = hint; i < frames; ++i) {
    if (!bit_test(i)) {
      bit_set(i);
      hint = i + 1;
      return base + static_cast<uintptr_t>(i * PageSize);
    }
  }

  for (size_t i = 0; i < hint; ++i) {
    if (!bit_test(i)) {
      bit_set(i);
      hint = i + 1;
      return base + static_cast<uintptr_t>(i * PageSize);
    }
  }

  return 0;
}

void BmPageFrameAllocator::free_frame(uintptr_t addr) noexcept {
  if (!in_managed(addr)) return;
  size_t i = index_of(addr);
  if (i >= frames) return;
  bit_clear(i);
  if (i < hint) hint = i;
}

}  // namespace mem::builtin
