#pragma once
#include <cstdint>

namespace gfx {

struct Color {
  uint8_t r;
  uint8_t g;
  uint8_t b;
  uint8_t a;

  constexpr Color() : r(0), g(0), b(0), a(255) {}

  constexpr Color(uint8_t r, uint8_t g, uint8_t b) : r(r), g(g), b(b), a(255) {}

  constexpr Color(uint8_t r, uint8_t g, uint8_t b, uint8_t a) : r(r), g(g), b(b), a(a) {}

  // From ARGB 0xAARRGGBB
  constexpr Color(uint32_t argb)
      : r((argb >> 16) & 0xFF), g((argb >> 8) & 0xFF), b((argb >> 0) & 0xFF), a((argb >> 24) & 0xFF) {}

  // Implicit conversion to 0xAARRGGBB
  constexpr operator uint32_t() const {
    return (uint32_t(a) << 24) | (uint32_t(r) << 16) | (uint32_t(g) << 8) | (uint32_t(b));
  }

  static constexpr Color Black() { return Color(0, 0, 0, 255); }

  static constexpr Color White() { return Color(255, 255, 255, 255); }

  static constexpr Color Red() { return Color(255, 0, 0, 255); }

  static constexpr Color Green() { return Color(0, 255, 0, 255); }

  static constexpr Color Blue() { return Color(0, 0, 255, 255); }

  static constexpr Color Yellow() { return Color(255, 255, 0, 255); }

  static constexpr Color Magenta() { return Color(255, 0, 255, 255); }

  static constexpr Color Cyan() { return Color(0, 255, 255, 255); }

  static constexpr Color Transparent() { return Color(0, 0, 0, 0); }

  static constexpr Color Gray(uint8_t v = 128) { return Color(v, v, v, 255); }
};

}  // namespace gfx
