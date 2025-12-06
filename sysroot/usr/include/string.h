#pragma once
#include <stddef.h>

extern "C" void* memset(void* dest, int ch, size_t count);
extern "C" void* memcpy(void* __restrict__ dest, const void* __restrict__ src,
                        size_t count);
extern "C" void* memmove(void* __restrict__ dest, const void* __restrict__ src,
                         size_t count);
extern "C" void* __memcpy_chk(void* __restrict__ dest, const void* __restrict__ src,
                              size_t len, size_t destlen);
extern "C" void* __memmove_chk(void* __restrict__ dest, const void* __restrict__ src,
                               size_t len, size_t destlen);
extern "C" size_t strlen(const char* start);
extern "C" int memcmp(const void* lhs, const void* rhs, size_t count);
extern "C" int strncmp(const char* lhs, const char* rhs, size_t count);
