#pragma once
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

int atexit(void (*)(void));
int atoi(const char*);

char* getenv(const char*);

void free(void* ptr);
void* malloc(size_t size);

void abort(void);

#ifdef __cplusplus
}
#endif
