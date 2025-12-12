#pragma once

#include <cstddef>
#include <cstdint>

namespace hal {

enum class PageFlags : uint32_t {
  None = 0,
  Present = 1u << 0,
  Writable = 1u << 1,
  User = 1u << 2,
  WriteThrough = 1u << 3,
  CacheDisable = 1u << 4,
  Global = 1u << 8
};

inline PageFlags operator|(PageFlags a, PageFlags b) noexcept {
  return static_cast<PageFlags>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline PageFlags operator&(PageFlags a, PageFlags b) noexcept {
  return static_cast<PageFlags>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

inline PageFlags& operator|=(PageFlags& a, PageFlags b) noexcept {
  a = a | b;
  return a;
}

inline PageFlags& operator&=(PageFlags& a, PageFlags b) noexcept {
  a = a & b;
  return a;
}

class Paging {
 public:
  virtual ~Paging() = default;

  virtual size_t page_size() const noexcept = 0;
  virtual bool map(uintptr_t vaddr, uintptr_t paddr, PageFlags flags) noexcept = 0;
  virtual bool map_range(uintptr_t vaddr, uintptr_t paddr, size_t pages,
                         PageFlags flags) noexcept = 0;
  virtual void unmap(uintptr_t vadddr) noexcept = 0;
  virtual void unmap_range(uintptr_t vaddr, size_t pages) noexcept = 0;
  virtual bool translate(uintptr_t vaddr, uintptr_t& out_paddr,
                         PageFlags& out_flags) const noexcept = 0;
  virtual void flush(uintptr_t vaddr) noexcept = 0;
  virtual void flush_all() noexcept = 0;
};

}  // namespace hal
