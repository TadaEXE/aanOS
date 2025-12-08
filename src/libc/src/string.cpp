#include <cstddef>
#include <cstdint>
#include <string.h>

extern "C" void* memset(void* dest, int ch, size_t count) {
  auto* p = static_cast<uint8_t*>(dest);
  auto v = static_cast<uint8_t>(ch);

  for (size_t i = 0; i < count; ++i) {
    p[i] = v;
  }

  return dest;
}

extern "C" void* memcpy(void* __restrict__ dest, const void* __restrict__ src,
                        size_t count) {
  if (reinterpret_cast<uintptr_t>(dest) % sizeof(size_t) == 0 &&
      reinterpret_cast<uintptr_t>(src) % sizeof(size_t) == 0 &&
      count % sizeof(size_t) == 0) {
    size_t* d = reinterpret_cast<size_t*>(dest);
    const size_t* s = reinterpret_cast<const size_t*>(src);

    for (size_t i = 0; i < count / sizeof(size_t); ++i) {
      d[i] = s[i];
    }
  } else {
    char* d = reinterpret_cast<char*>(dest);
    const char* s = reinterpret_cast<const char*>(src);
    for (size_t i = 0; i < count; ++i) {
      d[i] = s[i];
    }
  }

  return dest;
}

extern "C" void* memmove(void* __restrict__ dest, const void* __restrict__ src,
                         size_t count) {
  if (!dest || !src) return nullptr;

  if (reinterpret_cast<uintptr_t>(dest) % sizeof(size_t) == 0 &&
      reinterpret_cast<uintptr_t>(src) % sizeof(size_t) == 0 &&
      count % sizeof(size_t) == 0) {
    size_t* d = reinterpret_cast<size_t*>(dest);
    const size_t* s = reinterpret_cast<const size_t*>(src);
    count /= sizeof(size_t);

    if (dest <= src) {
      while (count--) {
        *d++ = *s++;
      }
    } else {
      d += count - 1;
      s += count - 1;
      while (count--) {
        *d-- = *s--;
      }
    }

  } else {
    char* d = reinterpret_cast<char*>(dest);
    const char* s = reinterpret_cast<const char*>(src);
    if (dest <= src) {
      while (count--) {
        *d++ = *s++;
      }
    } else {
      d += count - 1;
      s += count - 1;
      while (count--) {
        *d-- = *s--;
      }
    }
  }

  return dest;
}

/// This implementation does not conform to the specification. Which says:
/// "The parameter destlen specifies the size of the object dest. If len exceeds destlen, the function shall abort, and the program calling it shall exit."
/// Instead of aborting, len will be clamped to destlen.
extern "C" void* __memcpy_chk(void* __restrict__ dest, const void* __restrict__ src,
                              size_t len, size_t destlen) {
  if (len > destlen) len = destlen;
  return memcpy(dest, src, len);
}

/// This implementation does not conform to the specification. Which says:
/// "The parameter destlen specifies the size of the object dest. If len exceeds destlen, the function shall abort, and the program calling it shall exit."
/// Instead of aborting, len will be clamped to destlen.
extern "C" void* __memmove_chk(void* __restrict__ dest, const void* __restrict__ src,
                               size_t len, size_t destlen) {
  if (len > destlen) len = destlen;
  return memmove(dest, src, len);
}

extern "C" size_t strlen(const char* start) {
  if (!start) return 0;
  const char* end = start;
  while (*end != '\0')
    ++end;
  return end - start;
}

extern "C" int memcmp(const void* lhs, const void* rhs, size_t count) {
  auto* a = static_cast<const unsigned char*>(lhs);
  auto* b = static_cast<const unsigned char*>(rhs);

  for (size_t i = 0; i < count; ++i) {
    if (a[i] != b[i]) { return (a[i] < b[i]) ? -1 : 1; }
  }

  return 0;
}

extern "C" int strncmp(const char* lhs, const char* rhs, size_t count) {
  auto* a = static_cast<const char*>(lhs);
  auto* b = static_cast<const char*>(rhs);

  for (size_t i = 0; i < count; ++i) {
    if (a[i] != b[i]) { return (a[i] < b[i]) ? -1 : 1; }
  }

  return 0;
}

// namespace std {
//
// void* memset(void* dest, int ch, size_t count) {
//   return ::memset(dest, ch, count);
// }
//
// void* memcpy(void* dest, const void* src, size_t count) {
//   return ::memcpy(dest, src, count);
// }
//
// void* memmove(void* dest, const void* src, size_t count) {
//   return ::memmove(dest, src, count);
// }
//
// int memcmp(const void* lhs, const void* rhs, size_t count) {
//   return ::memcmp(lhs, rhs, count);
// }
//
// int strncmp(const char* lhs, const char* rhs, size_t count) {
//   return ::strncmp(lhs, rhs, count);
// }
//
// size_t strlen(const char* start) {
//   return ::strlen(start);
// }

// }  // namespace std
