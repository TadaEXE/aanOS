#include "gfx/shapes.hpp"

namespace gfx {

Point operator-(Point a, Point b) {
  a.x -= b.x;
  a.y -= b.y;
  return a;
}

Point operator+(Point a, Point b) {
  a.x += b.x;
  a.y += b.y;
  return a;
}

Point& Point::operator-=(int32_t t) {
  x -= t;
  y -= t;
  return *this;
}

Point& Point::operator+=(int32_t t) {
  x += t;
  y += t;
  return *this;
}

Point operator+(Point a, int32_t t) {
  a += t;
  return a;
}

Point operator-(Point a, int32_t t) {
  a -= t;
  return a;
}

Rect& Rect::operator-=(int32_t t) {
  x += t;
  y += t;
  w -= 2 * t;
  h -= 2 * t;
  return *this;
}

Rect& Rect::operator+=(int32_t t) {
  x -= t;
  y -= t;
  w += 2 * t;
  h += 2 * t;
  return *this;
}

Rect operator-(Rect r, int32_t t) {
  r -= t;
  return r;
}

Rect operator+(Rect r, int32_t t) {
  r += t;
  return r;
}

}  // namespace gfx
