#pragma once

#include <cstddef>
#include <cstdint>

// If someone ever finds this header (me in a week) and wants to know why all function are cryptic combinations of three letters...
// IDK

namespace bits {

static constexpr bool ipo2(size_t x) {
  return (x & (x - 1)) == 0;
}

static constexpr size_t oiz(size_t x) {
  return x ? x : x + 1;
}

uint32_t flp2(uint32_t x);
uint64_t flp2(uint64_t x);

uint32_t clp2(uint32_t x);
uint64_t clp2(uint64_t x);

}  // namespace bits
