#!/usr/bin/env python3
"""
Generate a C++ 8x16 bitmap font for ASCII 0x20..0x7E into a flat uint8_t array.

Usage:
    python3 fontgen.py > bitmap_font_builtin.cpp
"""

import sys

GLYPH_W = 5
GLYPH_H = 7
OUT_W = 8
OUT_H = 16

glyph_5x7: dict[str, list[str]] = {}

# Digits 0-9
glyph_5x7["0"] = [
    " ### ",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["1"] = [
    "  #  ",
    " ##  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    " ### ",
]

glyph_5x7["2"] = [
    " ### ",
    "#   #",
    "    #",
    "   # ",
    "  #  ",
    " #   ",
    "#####",
]

glyph_5x7["3"] = [
    " ### ",
    "#   #",
    "    #",
    " ### ",
    "    #",
    "#   #",
    " ### ",
]

glyph_5x7["4"] = [
    "#   #",
    "#   #",
    "#   #",
    "#####",
    "    #",
    "    #",
    "    #",
]

glyph_5x7["5"] = [
    "#####",
    "#    ",
    "#    ",
    "#### ",
    "    #",
    "#   #",
    " ### ",
]

glyph_5x7["6"] = [
    " ### ",
    "#   #",
    "#    ",
    "#### ",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["7"] = [
    "#####",
    "    #",
    "   # ",
    "   # ",
    "  #  ",
    "  #  ",
    "  #  ",
]

glyph_5x7["8"] = [
    " ### ",
    "#   #",
    "#   #",
    " ### ",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["9"] = [
    " ### ",
    "#   #",
    "#   #",
    " ####",
    "    #",
    "#   #",
    " ### ",
]

# Uppercase A-Z
glyph_5x7["A"] = [
    " ### ",
    "#   #",
    "#   #",
    "#####",
    "#   #",
    "#   #",
    "#   #",
]

glyph_5x7["B"] = [
    "#### ",
    "#   #",
    "#   #",
    "#### ",
    "#   #",
    "#   #",
    "#### ",
]

glyph_5x7["C"] = [
    " ### ",
    "#   #",
    "#    ",
    "#    ",
    "#    ",
    "#   #",
    " ### ",
]

glyph_5x7["D"] = [
    "#### ",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#### ",
]

glyph_5x7["E"] = [
    "#####",
    "#    ",
    "#    ",
    "#####",
    "#    ",
    "#    ",
    "#####",
]

glyph_5x7["F"] = [
    "#####",
    "#    ",
    "#    ",
    "#####",
    "#    ",
    "#    ",
    "#    ",
]

glyph_5x7["G"] = [
    " ### ",
    "#   #",
    "#    ",
    "# ###",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["H"] = [
    "#   #",
    "#   #",
    "#   #",
    "#####",
    "#   #",
    "#   #",
    "#   #",
]

glyph_5x7["I"] = [
    " ### ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    " ### ",
]

glyph_5x7["J"] = [
    "  ###",
    "   # ",
    "   # ",
    "   # ",
    "   # ",
    "#  # ",
    " ##  ",
]

glyph_5x7["K"] = [
    "#   #",
    "#  # ",
    "# #  ",
    "##   ",
    "# #  ",
    "#  # ",
    "#   #",
]

glyph_5x7["L"] = [
    "#    ",
    "#    ",
    "#    ",
    "#    ",
    "#    ",
    "#    ",
    "#####",
]

glyph_5x7["M"] = [
    "#   #",
    "## ##",
    "# # #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
]

glyph_5x7["N"] = [
    "#   #",
    "##  #",
    "##  #",
    "# # #",
    "#  ##",
    "#  ##",
    "#   #",
]

glyph_5x7["O"] = [
    " ### ",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["P"] = [
    "#### ",
    "#   #",
    "#   #",
    "#### ",
    "#    ",
    "#    ",
    "#    ",
]

glyph_5x7["Q"] = [
    " ### ",
    "#   #",
    "#   #",
    "#   #",
    "# # #",
    "#  # ",
    " ## #",
]

glyph_5x7["R"] = [
    "#### ",
    "#   #",
    "#   #",
    "#### ",
    "# #  ",
    "#  # ",
    "#   #",
]

glyph_5x7["S"] = [
    " ### ",
    "#   #",
    "#    ",
    " ### ",
    "    #",
    "#   #",
    " ### ",
]

glyph_5x7["T"] = [
    "#####",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
]

glyph_5x7["U"] = [
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    " ### ",
]

glyph_5x7["V"] = [
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    "#   #",
    " # # ",
    "  #  ",
]

glyph_5x7["W"] = [
    "#   #",
    "#   #",
    "#   #",
    "# # #",
    "# # #",
    "## ##",
    "#   #",
]

glyph_5x7["X"] = [
    "#   #",
    "#   #",
    " # # ",
    "  #  ",
    " # # ",
    "#   #",
    "#   #",
]

glyph_5x7["Y"] = [
    "#   #",
    "#   #",
    "#   #",
    " # # ",
    "  #  ",
    "  #  ",
    "  #  ",
]

glyph_5x7["Z"] = [
    "#####",
    "    #",
    "   # ",
    "  #  ",
    " #   ",
    "#    ",
    "#####",
]

# Basic punctuation
glyph_5x7["?"] = [
    " ### ",
    "#   #",
    "    #",
    "   # ",
    "  #  ",
    "     ",
    "  #  ",
]

glyph_5x7["!"] = [
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "  #  ",
    "     ",
    "  #  ",
]

glyph_5x7["."] = [
    "     ",
    "     ",
    "     ",
    "     ",
    "     ",
    "  #  ",
    "  #  ",
]

glyph_5x7[" "] = [
    "     ",
    "     ",
    "     ",
    "     ",
    "     ",
    "     ",
    "     ",
]

# Extra punctuation so everything in 0x20..0x7E has a real glyph
extra_punct = {
    '"': [
        " # # ",
        " # # ",
        " # # ",
        " # # ",
        "     ",
        "     ",
        "     ",
    ],
    '#': [
        " # # ",
        " # # ",
        "#####",
        " # # ",
        "#####",
        " # # ",
        " # # ",
    ],
    '$': [
        "  #  ",
        " ####",
        "# #  ",
        " ### ",
        "  # #",
        "#### ",
        "  #  ",
    ],
    '%': [
        "##   ",
        "##  #",
        "   # ",
        "  #  ",
        " #   ",
        "#  ##",
        "   ##",
    ],
    '&': [
        " ##  ",
        "#  # ",
        "#  # ",
        " ##  ",
        "# #  ",
        "#  # ",
        " ## #",
    ],
    "'": [
        "  #  ",
        "  #  ",
        "  #  ",
        " #   ",
        "     ",
        "     ",
        "     ",
    ],
    '(': [
        "   # ",
        "  #  ",
        "  #  ",
        " #   ",
        " #   ",
        "  #  ",
        "  #  ",
    ],
    ')': [
        " #   ",
        "  #  ",
        "  #  ",
        "   # ",
        "   # ",
        "  #  ",
        "  #  ",
    ],
    '*': [
        "     ",
        " # # ",
        " ### ",
        "#####",
        " ### ",
        " # # ",
        "     ",
    ],
    '+': [
        "     ",
        "  #  ",
        "  #  ",
        "#####",
        "  #  ",
        "  #  ",
        "     ",
    ],
    ',': [
        "     ",
        "     ",
        "     ",
        "     ",
        "  #  ",
        "  #  ",
        " #   ",
    ],
    '-': [
        "     ",
        "     ",
        "     ",
        "#####",
        "     ",
        "     ",
        "     ",
    ],
    '/': [
        "    #",
        "   # ",
        "   # ",
        "  #  ",
        "  #  ",
        " #   ",
        "#    ",
    ],
    ':': [
        "     ",
        "  #  ",
        "  #  ",
        "     ",
        "  #  ",
        "  #  ",
        "     ",
    ],
    ';': [
        "     ",
        "  #  ",
        "  #  ",
        "     ",
        "  #  ",
        "  #  ",
        " #   ",
    ],
    '<': [
        "   # ",
        "  #  ",
        " #   ",
        " #   ",
        "  #  ",
        "   # ",
        "     ",
    ],
    '=': [
        "     ",
        "#####",
        "     ",
        "#####",
        "     ",
        "     ",
        "     ",
    ],
    '>': [
        " #   ",
        "  #  ",
        "   # ",
        "   # ",
        "  #  ",
        " #   ",
        "     ",
    ],
    '@': [
        " ### ",
        "#   #",
        "# # #",
        "# ###",
        "#    ",
        "#    ",
        " ### ",
    ],
    '[': [
        " ### ",
        " #   ",
        " #   ",
        " #   ",
        " #   ",
        " #   ",
        " ### ",
    ],
    '\\': [
        "#    ",
        " #   ",
        " #   ",
        "  #  ",
        "  #  ",
        "   # ",
        "    #",
    ],
    ']': [
        " ### ",
        "   # ",
        "   # ",
        "   # ",
        "   # ",
        "   # ",
        " ### ",
    ],
    '^': [
        "  #  ",
        " # # ",
        "#   #",
        "     ",
        "     ",
        "     ",
        "     ",
    ],
    '_': [
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "     ",
        "#####",
    ],
    '`': [
        " #   ",
        "  #  ",
        "   # ",
        "     ",
        "     ",
        "     ",
        "     ",
    ],
    '{': [
        "   ##",
        "  #  ",
        "  #  ",
        "##   ",
        "  #  ",
        "  #  ",
        "   ##",
    ],
    '|': [
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
        "  #  ",
    ],
    '}': [
        "##   ",
        "  #  ",
        "  #  ",
        "   ##",
        "  #  ",
        "  #  ",
        "##   ",
    ],
    '~': [
        "     ",
        "     ",
        " ## #",
        "# ## ",
        "     ",
        "     ",
        "     ",
    ],
}

glyph_5x7.update(extra_punct)


def row_to_byte(row: str) -> int:
    if len(row) != GLYPH_W:
        raise ValueError(f"Row {row!r} has length {len(row)}, expected {GLYPH_W}")
    byte = 0
    for i, ch in enumerate(row):
        if ch != " ":
            bit_index = 6 - i  # use bits 6..2 for 5 pixels
            byte |= 1 << bit_index
    return byte


def glyph_to_8x16_rows(ch: str) -> list[int]:
    if ch not in glyph_5x7:
        if "A" <= ch <= "Z":
            pattern = glyph_5x7.get(ch, glyph_5x7["?"])
        elif "a" <= ch <= "z":
            pattern = glyph_5x7.get(ch.upper(), glyph_5x7["?"])
        elif ch == " ":
            pattern = glyph_5x7[" "]
        else:
            pattern = glyph_5x7["?"]
    else:
        pattern = glyph_5x7[ch]

    rows16: list[int] = []
    rows16.append(0)
    for r in pattern:
        b = row_to_byte(r)
        rows16.append(b)
        rows16.append(b)
    rows16.append(0)
    if len(rows16) != OUT_H:
        raise ValueError(f"Got {len(rows16)} rows for {ch!r}, expected {OUT_H}")
    return rows16


def main() -> None:
    first_char = 32
    last_char = 126
    glyph_height = OUT_H

    glyph_count = last_char - first_char + 1
    total_bytes = glyph_count * glyph_height

    sys.stdout.write('#include "bitmap_font_builtin.hpp"\n\n')
    sys.stdout.write("namespace {\n\n")

    sys.stdout.write(f"static const uint8_t builtin_font_data[{total_bytes}] = {{\n")

    for code in range(first_char, last_char + 1):
        ch = chr(code)
        rows = glyph_to_8x16_rows(ch)
        label = ch
        if ch == "\\":
            label = "\\\\"
        sys.stdout.write(f"    /* '{label}' (0x{code:02X}) */\n")
        for b in rows:
            sys.stdout.write(f"    0b{b:08b},\n")

    sys.stdout.write("};\n\n")
    sys.stdout.write(
        "static const BitmapFont builtin_desc{\n"
        '    "builtin_8x16",\n'
        "    8,\n"
        "    16,\n"
        "    ' ',\n"
        "    '~',\n"
        "    builtin_font_data,\n"
        "};\n\n"
    )
    sys.stdout.write("}  // namespace\n\n")
    sys.stdout.write(
        "const BitmapFont& builtin_font_8x16() noexcept {\n"
        "  return builtin_desc;\n"
        "}\n"
    )


if __name__ == "__main__":
    main()

