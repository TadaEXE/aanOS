#include "gfx/shapes.hpp"

namespace gfx {

Rect& Rect::operator-=(int32_t t) {
  x += t;
  y += t;
  w -= t;
  h -= t;
  return *this;
}

Rect& Rect::operator+=(int32_t t) {
  x -= t;
  y -= t;
  w += t;
  h += t;
  return *this;
}

Rect operator-(const Rect& r, int32_t t) {
  Rect res = r;
  res -= t;
  return res;
}

Rect operator+(const Rect& r, int32_t t) {
  Rect res = r;
  res += t;
  return res;
}

}  // namespace gfx
