#pragma once

#include <cstddef>
#include <type_traits>

namespace mem {

enum class SeekType {
  Forward,
  Backward,
  FromStart,
  FromEnd,
};

/// @brief Lightweight non-owning typed wrapper around any sequential data.
template <typename T>
  requires std::is_trivially_copyable_v<T>
class DataView {
 public:
  virtual ~DataView() = default;

  DataView(const DataView&) = delete;
  DataView(DataView&&) = delete;
  DataView& operator=(const DataView&) = delete;
  DataView& operator=(DataView&&) = delete;

  DataView(T* begin, size_t length) : begin(begin), length(length) {}

  virtual bool seek(size_t amount, SeekType type) noexcept {
    auto np = new_pos(amount, type);
    if (np < 0) return false;

    pos = static_cast<size_t>(np);
    return true;
  }

  virtual T* read() noexcept { return pos < length ? &begin[pos++] : nullptr; }

  virtual bool write(const T& item) noexcept {
    if (this->pos >= length && !grow_to(pos + 1)) return false;

    begin[pos++] = item;
    return true;
  }

  virtual size_t count() const noexcept { return length; }

  virtual size_t position() const noexcept { return pos; }

 protected:
  virtual bool grow_to(size_t) noexcept { return false; }

  ptrdiff_t new_pos(size_t amount, SeekType type) const noexcept {
    ptrdiff_t out = static_cast<ptrdiff_t>(pos);

    switch (type) {
      case SeekType::Forward:
        out += static_cast<ptrdiff_t>(amount);
        break;
      case SeekType::Backward:
        out -= static_cast<ptrdiff_t>(amount);
        break;
      case SeekType::FromStart:
        out = static_cast<ptrdiff_t>(amount);
        break;
      case SeekType::FromEnd:
        if (amount >= length) return -1;

        out = static_cast<ptrdiff_t>(length) - static_cast<ptrdiff_t>(amount) - 1;
        break;
    }

    return out;
  }

  T* begin;
  size_t length;

  size_t pos{0};
};

}  // namespace mem
