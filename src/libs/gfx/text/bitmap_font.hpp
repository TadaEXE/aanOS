#pragma once

#include <cstdint>

namespace gfx::text {

struct BitmapFont {
  uint32_t glyph_width;
  uint32_t glyph_height;
  char first_char;
  char last_char;
  const uint8_t* data;
};

const BitmapFont& builtin_font() noexcept;

}  // namespace gfx::text
