#pragma once

#include <cstddef>
#include <cstdint>

namespace ctr {

class TextLineBuffer {
public:
private:
  size_t line_count;
  size_t line_length;
  size_t line_height;

  size_t edit_line_idx;
};

}
