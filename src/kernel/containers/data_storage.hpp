#pragma once

#include <cstddef>
#include <cstring>
#include <type_traits>

#include "kernel/memory/data_view.hpp"
#include "kernel/memory/heap.hpp"
#include "math/bit_logic.hpp"

namespace ctr {

/// @brief Stack-based owing implementation of non-owning DataView
template <typename T, size_t Size>
  requires std::is_trivially_copyable_v<T>
class StackStorage : public mem::DataView<T> {
 public:
  StackStorage() : mem::DataView<T>(buffer, Size) {}

  StackStorage(const StackStorage&) = delete;
  StackStorage(StackStorage&&) = delete;
  StackStorage& operator=(const StackStorage&) = delete;
  StackStorage& operator=(StackStorage&&) = delete;

 private:
  T buffer[Size];
};

/// @brief Heap-based owing implementation of non-owning DataView
template <typename T>
  requires std::is_trivially_copyable_v<T>
class HeapStorage : public mem::DataView<T> {
 public:
  explicit HeapStorage(size_t initial_capacity = 64)
      : mem::DataView<T>(buffer, bits::oiz(bits::clp2(initial_capacity))),
        capacity(bits::oiz(bits::clp2(initial_capacity))),
        buffer(mem::alloc<T>(capacity, alignof(T))) {
    this->begin = buffer;
  }

  HeapStorage(const HeapStorage&) = delete;
  HeapStorage(HeapStorage&&) = delete;
  HeapStorage& operator=(const HeapStorage&) = delete;
  HeapStorage& operator=(HeapStorage&&) = delete;

 protected:
  bool grow_to(size_t len) noexcept override {
    if (len <= capacity) {
      this->length = len;
      return true;
    }

    auto nc = bits::clp2(capacity + 1);
    T* tmp = mem::alloc<T>(nc, alignof(T));
    if (!tmp) return false;
    memcpy(tmp, buffer, this->length);

    capacity = nc;
    buffer = tmp;
    return true;
  }

 private:
  size_t capacity{};
  T* buffer{};
};
}  // namespace ctr
