#pragma once
#include <cstdint>

#include "gfx/canvas.hpp"
#include "gfx/color.hpp"
#include "gfx/text/bitmap_font.hpp"

namespace gfx::text {

struct Style {
  Color fg;
  Color bg;
  bool draw_bg;
  uint32_t scale{1};
  int32_t gap_x{0};
  int32_t gap_y{0};
};

class TextRenderer {
 public:
  TextRenderer(const TextRenderer&) = default;
  TextRenderer(TextRenderer&&) = default;
  TextRenderer& operator=(const TextRenderer&) = delete;
  TextRenderer& operator=(TextRenderer&&) = delete;

  TextRenderer(Canvas& canvas, Style style, BitmapFont& font)
      : font(font), canvas(canvas), style(style) {}

  TextRenderer(Canvas& canvas, Style style)
      : font(builtin_font()), canvas(canvas), style(style) {}

  void draw_glyph(char c, bool inverted = false);
  void draw_glyph(char c, uint32_t x, uint32_t y, bool inverted = false);

  void draw_text(const char* text, bool inverted = false);
  void draw_text(const char* text, uint32_t x, uint32_t y, bool inverted = false);

  void set_pos(uint32_t x, uint32_t y);

  void set_style(Style style) { this->style = style; }

  const BitmapFont& font;

 private:
  Canvas& canvas;
  Style style;
  uint32_t last_x{0};
  uint32_t last_y{0};
};

}  // namespace gfx::text
