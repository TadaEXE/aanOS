#pragma once
#include <cstddef>
#include <cstdint>
#include <type_traits>

namespace ctr {

template <typename Key, typename Val, size_t Capacity>
  requires std::is_default_constructible_v<Key> && std::is_default_constructible_v<Val>
class FlatMap {
 public:
  FlatMap() = default;
  FlatMap(const FlatMap&) = delete;
  FlatMap(FlatMap&&) = delete;
  FlatMap& operator=(const FlatMap&) = delete;
  FlatMap& operator=(FlatMap&&) = delete;

  bool insert(const Key& key, const Val& val) noexcept {
    if (full()) return false;

    if (auto* v = find(key)) {
      *v = val;
      return true;
    }

    auto h = hash(key) % Capacity;
    Entry& e = data[h];

    if (!e.taken) {
      e = {key, val, true};
      ++item_count;
    } else if (e.key == key) {
      e.value = val;
    } else {
      auto* p = probe_entry_linear(key, h);
      *p = {key, val, true};
      ++item_count;
    }

    return true;
  }

  Val* find(const Key& key) noexcept {
    if (empty()) return nullptr;

    uintptr_t h = hash(key) % Capacity;
    Entry& e = data[h];
    if (e.taken && e.key == key) { return &e.value; }

    auto* p = probe_entry_linear(key, h);
    return (p) ? nullptr : &p->value;
  }

  void remove(const Key& key) noexcept {
    if (empty()) return;

    uintptr_t h = hash(key) % Capacity;
    auto& e = data[h];
    if (e.taken && e.key == key) {
      e.taken = false;
      --item_count;
    } else if (auto* p = probe_entry_linear(key, h)) {
      p->taken = false;
      --item_count;
    }
  }

  inline size_t count() const noexcept { return item_count; }

  inline size_t size() const noexcept { return Capacity; }

  inline bool empty() const noexcept { return item_count == 0; }

  inline bool full() const noexcept { return item_count == Capacity; }

  struct Entry {
    Key key;
    Val value;
    bool taken{false};
  };

  Entry data[Capacity];

 private:
  size_t item_count{0};

  Entry* probe_entry_linear(Key key, uintptr_t h, uintptr_t* io = nullptr) noexcept {
    size_t i = (h + 1) % Capacity;
    Entry* e = &data[i];
    while (i != h) {
      if ((e->taken && e->key == key) || !e->taken) {
        if (io) *io = i;
        return e;
      }
      i = (i + 1) % Capacity;
      e = &data[i];
    }
    return nullptr;
  }

  Entry* probe_entry_quadrtic(Key key, uintptr_t h) noexcept {
    uint32_t k = 1;
    size_t i = (h + k) % Capacity;
    auto* e = &data[i];
    while (i != h) {
      if ((e->taken && e->key == key) || !e->taken) { return e; }
      k <<= 1;
      i = (h + k) % Capacity;
      e = &data[i];
    }
    return e;
  }

  static constexpr uintptr_t mix_uintptr(uintptr_t x) noexcept {
    if constexpr (sizeof(uintptr_t) == 8) {
      // 64-bit finalizer (from MurmurHash3)
      uint64_t h = static_cast<uint64_t>(x);
      h ^= h >> 33;
      h *= 0xff51afd7ed558ccdULL;
      h ^= h >> 33;
      h *= 0xc4ceb9fe1a85ec53ULL;
      h ^= h >> 33;
      return static_cast<uintptr_t>(h);
    } else {
      // 32-bit finalizer (from MurmurHash3)
      uint32_t h = static_cast<uint32_t>(x);
      h ^= h >> 16;
      h *= 0x85ebca6bU;
      h ^= h >> 13;
      h *= 0xc2b2ae35U;
      h ^= h >> 16;
      return static_cast<uintptr_t>(h);
    }
  }

  uintptr_t hash(const Key& key) const noexcept {
    if constexpr (std::is_integral_v<Key> || std::is_enum_v<Key> ||
                  std::is_pointer_v<Key>) {
      uintptr_t x = static_cast<uintptr_t>(key);
      return mix_uintptr(x);
    } else {
      // FNV-1a over the object bytes as a fallback
      const unsigned char* p = reinterpret_cast<const unsigned char*>(&key);
      uintptr_t h;
      if constexpr (sizeof(uintptr_t) == 8) {
        h = 14695981039346656037ULL;
        constexpr uint64_t prime = 1099511628211ULL;
        for (size_t i = 0; i < sizeof(Key); ++i) {
          h ^= static_cast<uintptr_t>(p[i]);
          h *= prime;
        }
      } else {
        h = 2166136261U;
        constexpr uintptr_t prime = 16777619U;
        for (size_t i = 0; i < sizeof(Key); ++i) {
          h ^= static_cast<uintptr_t>(p[i]);
          h *= prime;
        }
      }
      return h;
    }
  }
};
}  // namespace ctr
