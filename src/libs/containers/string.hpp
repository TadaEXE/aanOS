#pragma once

#include <cstddef>
#include <cstring>
#include <string_view>

#include "kernel/memory/heap.hpp"

namespace ctr {

class String {
 public:
  static constexpr size_t npos = static_cast<size_t>(-1);

  String() noexcept : buffer(nullptr), length(0), cap(0) {}

  explicit String(size_t reserve_capacity) : buffer(nullptr), length(0), cap(0) {
    if (reserve_capacity > 0) { reserve(reserve_capacity); }
  }

  String(const char* cstr) : buffer(nullptr), length(0), cap(0) {
    if (!cstr) { return; }
    size_t len = std::strlen(cstr);
    if (len == 0) { return; }
    reserve(len);
    memmove(buffer, cstr, len);
    length = len;
    buffer[length] = '\0';
  }

  String(std::string_view sv) : buffer(nullptr), length(0), cap(0) {
    if (sv.empty()) { return; }
    reserve(sv.size());
    memmove(buffer, sv.data(), sv.size());
    length = sv.size();
    buffer[length] = '\0';
  }

  String(const String& other) : buffer(nullptr), length(0), cap(0) {
    if (other.length == 0) { return; }
    reserve(other.length);
    memmove(buffer, other.buffer, other.length);
    length = other.length;
    buffer[length] = '\0';
  }

  String(String&& other) noexcept
      : buffer(other.buffer), length(other.length), cap(other.cap) {
    other.buffer = nullptr;
    other.length = 0;
    other.cap = 0;
  }

  ~String() { mem::free(buffer); }

  String& operator=(const String& other) {
    if (this == &other) { return *this; }

    if (other.length == 0) {
      clear();
      return *this;
    }

    if (other.length > cap) {
      char* new_data = mem::alloc<char>(other.length + 1);
      mem::free(buffer);
      buffer = new_data;
      cap = other.length;
    }

    memmove(buffer, other.buffer, other.length);
    length = other.length;
    buffer[length] = '\0';
    return *this;
  }

  String& operator=(String&& other) noexcept {
    if (this == &other) { return *this; }

    mem::free(buffer);

    buffer = other.buffer;
    length = other.length;
    cap = other.cap;

    other.buffer = nullptr;
    other.length = 0;
    other.cap = 0;

    return *this;
  }

  String& operator=(std::string_view sv) {
    if (sv.data() >= buffer && sv.data() < buffer + cap) {
      if (sv.size() == length && sv.data() == buffer) { return *this; }
      String tmp(sv);
      *this = std::move(tmp);
      return *this;
    }

    clear();
    if (sv.empty()) { return *this; }

    if (sv.size() > cap) {
      char* new_data = mem::alloc<char>(sv.size() + 1);
      mem::free(buffer);
      buffer = new_data;
      cap = sv.size();
    }

    memmove(buffer, sv.data(), sv.size());
    length = sv.size();
    buffer[length] = '\0';
    return *this;
  }

  const char* c_str() const noexcept { return buffer ? buffer : empty_cstr(); }

  const char* data() const noexcept { return c_str(); }

  char* raw_data() noexcept { return buffer; }

  size_t size() const noexcept { return length; }

  bool empty() const noexcept { return length == 0; }

  size_t capacity() const noexcept { return cap; }

  char& operator[](size_t index) noexcept { return buffer[index]; }

  const char& operator[](size_t index) const noexcept { return buffer[index]; }

  void clear() noexcept {
    length = 0;
    if (buffer) { buffer[0] = '\0'; }
  }

  void reserve(size_t new_cap) {
    if (new_cap <= cap) { return; }

    if (new_cap < length) { new_cap = length; }

    char* new_data = mem::alloc<char>(new_cap + 1);

    if (buffer && length > 0) { memmove(new_data, buffer, length); }

    new_data[length] = '\0';

    mem::free(buffer);
    buffer = new_data;
    cap = new_cap;
  }

  void push_back(char c) {
    if (length + 1 > cap) { grow_for(1); }

    buffer[length++] = c;
    buffer[length] = '\0';
  }

  void pop_back() noexcept {
    if (length == 0 || buffer == nullptr) { return; }
    --length;
    buffer[length] = '\0';
  }

  void append(std::string_view sv) {
    if (sv.empty()) { return; }

    if (length + sv.size() > cap) { grow_for(sv.size()); }

    memmove(buffer + length, sv.data(), sv.size());
    length += sv.size();
    buffer[length] = '\0';
  }

  void append(const char* cstr) {
    if (!cstr) { return; }
    size_t len = std::strlen(cstr);
    append(std::string_view{cstr, len});
  }

  String& operator+=(char c) {
    push_back(c);
    return *this;
  }

  String& operator+=(std::string_view sv) {
    append(sv);
    return *this;
  }

  bool equals(std::string_view sv) const noexcept {
    if (sv.size() != length) { return false; }
    if (length == 0) { return true; }
    return std::memcmp(buffer, sv.data(), length) == 0;
  }

 private:
  static const char* empty_cstr() noexcept {
    static const char empty = '\0';
    return &empty;
  }

  void grow_for(size_t extra) {
    size_t required = length + extra;
    size_t new_cap = cap == 0 ? required : cap * 2;
    if (new_cap < required) { new_cap = required; }
    reserve(new_cap);
  }

  char* buffer;
  size_t length;
  size_t cap;
};

inline bool operator==(const String& lhs, std::string_view rhs) noexcept {
  return lhs.equals(rhs);
}

inline bool operator==(std::string_view lhs, const String& rhs) noexcept {
  return rhs.equals(lhs);
}

inline bool operator!=(const String& lhs, std::string_view rhs) noexcept {
  return !lhs.equals(rhs);
}

inline bool operator!=(std::string_view lhs, const String& rhs) noexcept {
  return !rhs.equals(lhs);
}

}  // namespace ctr
