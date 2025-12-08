#include <cstdint>

extern "C" {

uint64_t __udivmoddi4(uint64_t num, uint64_t den, uint64_t* rem) {
  if (den == 0) {
    // Division by zero is UB in C/C++; choose a simple trap behavior.
    // For now: set remainder to 0 and return 0.
    if (rem) {
      *rem = 0;
    }
    return 0;
  }

  uint64_t quotient = 0;

  while (num >= den) {
    uint64_t tmp = den;
    uint64_t mult = 1;

    // Find the largest tmp = den * 2^k that is <= num, without overflow.
    while ((tmp <= (num >> 1)) && (tmp <= (UINT64_MAX >> 1))) {
      tmp <<= 1;
      mult <<= 1;
    }

    num -= tmp;
    quotient += mult;
  }

  if (rem) {
    *rem = num;
  }

  return quotient;
}
}
