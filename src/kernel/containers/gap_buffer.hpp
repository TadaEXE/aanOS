#pragma once

#include <cstddef>
#include <cstring>

#include "kernel/memory/heap.hpp"
#include "kernel/panic.hpp"
#include "math/bit_logic.hpp"

namespace ctr {

namespace {
template <typename T>
constexpr size_t min_gap_size() {
  return bits::flp2(bits::oiz(32 / sizeof(T)));
}
}  // namespace

template <typename T, size_t GapSize = min_gap_size<T>()>
class GapBuffer {
 public:
  explicit GapBuffer(size_t initial_capacity = GapSize * 2)
      : length(0),
        capacity(initial_capacity < GapSize ? GapSize * 2 : initial_capacity),
        begin(mem::alloc<T>(capacity, alignof(T))),
        gap_begin(begin),
        gap_end(gap_begin + GapSize) {
    if (!begin) panic("Allocation of GapBuffer failed");
  }

  GapBuffer(const GapBuffer&) = delete;
  GapBuffer(GapBuffer&&) = delete;
  GapBuffer& operator=(const GapBuffer&) = delete;
  GapBuffer& operator=(GapBuffer&&) = delete;

  ~GapBuffer() { mem::free(begin); }

  const T& operator[](size_t idx) const {
    if (idx >= length) {
      panic("Out of range access of gap buffer at %u with length %u.", idx, length);
    }

    size_t prefix_len = static_cast<size_t>(gap_begin - begin);
    if (idx < prefix_len) {
      return begin[idx];
    } else {
      return gap_end[idx - prefix_len];
    }
  }

  void insert(const T* items, size_t count = 1) {
    if (!items) return;
    if (!count) return;

    if (gap_width() >= count) {
      for (size_t i = 0; i < count; ++i) {
        *gap_begin++ = items[i];
      }
      length += count;
      return;
    }

    grow(count);
    insert(items, count);
  }

  void move_left(size_t count = 1) {
    if (!count) return;

    size_t prefix_len = static_cast<size_t>(gap_begin - begin);
    if (count > prefix_len) { count = prefix_len; }
    if (!count) return;

    memmove(gap_end - count, gap_begin - count, count * sizeof(T));
    gap_begin -= count;
    gap_end -= count;
  }

  void delete_left(size_t count = 1) {
    if (!count) return;

    size_t prefix_len = static_cast<size_t>(gap_begin - begin);
    if (count > prefix_len) { count = prefix_len; }
    if (!count) return;

    gap_begin -= count;
    length -= count;
  }

  void move_right(size_t count = 1) {
    if (!count) return;

    size_t suffix_len = length - static_cast<size_t>(gap_end - begin);
    if (count > suffix_len) { count = suffix_len; }
    if (!count) return;

    memmove(gap_begin, gap_end, count * sizeof(T));
    gap_begin += count;
    gap_end += count;
  }

  void delete_right(size_t count = 1) {
    if (!count) return;

    size_t suffix_len = length - static_cast<size_t>(gap_end - begin);
    if (count > suffix_len) { count = suffix_len; }
    if (!count) return;

    gap_end += count;
    length -= count;
  }

  void move_to(size_t index) {
    if (index > length) { index = length; }

    size_t cp = cursor_pos();
    if (index < cp) {
      move_left(cp - index);
    } else if (index > cp) {
      move_right(index - cp);
    }
  }

  size_t cursor_pos() const { return static_cast<size_t>(gap_begin - begin); }

  size_t count() const { return length; }

 private:
  void grow(size_t extend) {
    if (!extend) return;

    size_t new_cap = bits::clp2(length + extend + GapSize);

    size_t prefix_len = static_cast<size_t>(gap_begin - begin);
    size_t suffix_len = length - prefix_len;
    size_t new_gap_begin_off = prefix_len;
    size_t new_gap_size = extend < GapSize ? GapSize : extend;
    size_t new_gap_end_off = new_gap_begin_off + new_gap_size;

    if (new_cap <= capacity) {
      memmove(begin + new_gap_end_off, gap_end, suffix_len * sizeof(T));
      gap_end = begin + new_gap_end_off;
      return;
    }

    if (T* tmp = mem::alloc<T>(new_cap, alignof(T))) {
      memmove(tmp, begin, prefix_len * sizeof(T));
      memmove(tmp + new_gap_end_off, gap_end, suffix_len * sizeof(T));

      mem::free(begin);

      capacity = new_cap;
      begin = tmp;
      gap_begin = begin + new_gap_begin_off;
      gap_end = begin + new_gap_end_off;
      return;
    }

    panic("Malloc failed. (extend: %u, cap: %u, new_cap: %u, length: %u)", extend,
          capacity, new_cap, length);
  }

  size_t length{0};
  size_t capacity{0};

  T* begin{nullptr};
  T* gap_begin{nullptr};
  T* gap_end{nullptr};

  size_t gap_width() const { return static_cast<size_t>(gap_end - gap_begin); }
};

}  // namespace ctr
