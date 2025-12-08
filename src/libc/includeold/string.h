#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* memset(void* dest, int ch, size_t count);
void* memcpy(void* __restrict__ dest, const void* __restrict__ src, size_t count);
void* memmove(void* __restrict__ dest, const void* __restrict__ src, size_t count);
void* __memcpy_chk(void* __restrict__ dest, const void* __restrict__ src, size_t len,
                   size_t destlen);
void* __memmove_chk(void* __restrict__ dest, const void* __restrict__ src, size_t len,
                    size_t destlen);
size_t strlen(const char* start);
int memcmp(const void* lhs, const void* rhs, size_t count);
int strncmp(const char* lhs, const char* rhs, size_t count);

#ifdef __cplusplus
}
#endif
