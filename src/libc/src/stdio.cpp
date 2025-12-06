#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

int printf(const char* __restrict, ...) {
  return 0;
}

int putchar(int) {
  return 0;
}

int puts(const char*) {
  return 0;
}

#ifdef __cplusplus
}
#endif
