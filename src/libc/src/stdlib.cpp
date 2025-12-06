#include <stdlib.h>

#include "kernel/memory/heap.hpp"
#include "kernel/panic.hpp"

#ifdef __cplusplus
extern "C" {
#endif

void free(void* ptr) {
  mem::free(ptr);
}

void* malloc(size_t size) {
  return mem::alloc(size);
}

void abort(void) {
  panic("abort();");
}

#ifdef __cplusplus
}
#endif
