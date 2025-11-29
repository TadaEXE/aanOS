#pragma once
#include <cstdint>

namespace gfx {

struct Line {
  uint32_t s;
  uint32_t e;
  uint32_t w;
};

struct Rect {
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;

  Rect& operator-=(int32_t thickness);
  Rect& operator+=(int32_t thickness);
};

Rect operator+(const Rect& r, int32_t thickness);
Rect operator-(const Rect& r, int32_t thickness);

struct Sphere {
  uint32_t x;
  uint32_t y;
  uint32_t r;
};

}  // namespace gfx
