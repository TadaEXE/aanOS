#include "gfx/text/textrenderer.hpp"

#include <cstdint>

#include <kernel/log.hpp>

namespace gfx::text {

void TextRenderer::draw_glyph(char c, bool inverted) {
  if (style.scale == 0) return;
  if (c < font.first_char || c > font.last_char) return;

  uint32_t index = static_cast<uint32_t>(c - font.first_char);
  const uint8_t* glyph = font.data + index * font.glyph_height;

  for (uint32_t gy = 0; gy < font.glyph_height; ++gy) {
    uint8_t row = glyph[gy];
    row = inverted ? ~row : row;

    uint32_t py = (last_y + gy) * style.scale;

    canvas.draw_byte(last_x, py, row, style.fg, style.bg);
  }

  last_x += (font.glyph_width + style.gap_x) * style.scale;
}

void TextRenderer::draw_glyph(char c, uint32_t x, uint32_t y, bool inverted) {
  if (style.scale == 0) return;
  if (c < font.first_char || c > font.last_char) return;

  last_x = x;
  last_y = y;
  draw_glyph(c, inverted);
}

void TextRenderer::draw_text(const char* text, bool inverted) {
  if (!text || style.scale == 0) return;

  while (*text) {
    char c = *text++;
    draw_glyph(c, inverted);
  }
}

void TextRenderer::draw_text(const char* text, uint32_t x, uint32_t y, bool inverted) {
  if (!text || style.scale == 0) return;

  last_x = x;
  last_y = y;
  draw_text(text, inverted);
}

void TextRenderer::set_pos(uint32_t x, uint32_t y) {
  last_x = x;
  last_y = y;
}
}  // namespace gfx::text
