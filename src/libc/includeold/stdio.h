#pragma once

#include <stdarg.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#define EOF (-1)
#define stderr stderr
#define SEEK_SET 0

typedef struct {
  int unused;
} FILE;

extern FILE* stderr;

int printf(const char* __restrict, ...);
int putchar(int);
int puts(const char*);
int fclose(FILE*);
int fflush(FILE*);
FILE* fopen(const char*, const char*);
int fprintf(FILE*, const char*, ...);
size_t fread(void*, size_t, size_t, FILE*);
int fseek(FILE*, long, int);
long ftell(FILE*);
size_t fwrite(const void*, size_t, size_t, FILE*);
void setbuf(FILE*, char*);
int vfprintf(FILE*, const char*, va_list);

#ifdef __cplusplus
}
#endif
