#pragma once
#include <cstdint>

#include "kernel/logging/logging.hpp"

namespace gfx {

struct Point : public logging::Loggable {
  uint32_t x;
  uint32_t y;

  Point() = default;

  Point(uint32_t x, uint32_t y) : x(x), y(y) {}

  static constexpr const char* fmt() noexcept { return "{x=%u, y=%u}"; }

  bool operator==(const Point& other) const noexcept {
    return x == other.x && y == other.y;
  }

  bool operator!=(const Point& other) const noexcept { return !operator==(other); }

  Point& operator-=(int32_t t);
  Point& operator+=(int32_t t);

  void log_self() const noexcept override { log_obj<Point>(x, y); }
};

Point operator-(Point a, Point b);
Point operator+(Point a, Point b);

Point operator-(Point a, int32_t t);
Point operator+(Point a, int32_t t);

struct Line : public logging::Loggable {
  uint32_t s;
  uint32_t e;
  uint32_t w;
};

struct Rect : public logging::Loggable {
  uint32_t x;
  uint32_t y;
  uint32_t w;
  uint32_t h;

  Rect(const Rect&) = default;
  Rect(Rect&&) = default;
  Rect& operator=(const Rect&) = default;
  Rect& operator=(Rect&&) = default;

  bool operator==(const Rect& other) const {
    return other.x == x && other.y == y && other.w == w && other.h == h;
  }

  bool operator!=(const Rect& other) const { return !operator==(other); }

  constexpr Rect(uint32_t x, uint32_t y, uint32_t w, uint32_t h)
      : x(x), y(y), w(w), h(h) {}

  uint32_t end_x() const { return x + w; }

  uint32_t end_y() const { return y + h; }

  Rect& operator-=(int32_t thickness);
  Rect& operator+=(int32_t thickness);

  void log_self() const noexcept override { log_obj<Rect>(x, y, w, h); }

  static constexpr const char* fmt() noexcept { return "{x=%u, y=%u, w=%u, h=%u}"; }

  /// Inclusive start, exclusive end: [x, x+w), [y, y+h)
  bool is_inbounds(uint32_t px, uint32_t py) const noexcept {
    return (px >= x && px < x + w) && (py >= y && py < y + h);
  }

  bool is_inbounds(const Point& p) const noexcept { return is_inbounds(p.x, p.y); }

  static constexpr Rect Empty() { return Rect(0, 0, 0, 0); }
};

Rect operator+(Rect r, int32_t thickness);
Rect operator-(Rect r, int32_t thickness);

struct Sphere {
  uint32_t x;
  uint32_t y;
  uint32_t r;
};

}  // namespace gfx
