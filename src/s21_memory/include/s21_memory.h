#pragma once

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

void* s21_malloc(size_t size);

void* s21_calloc(size_t n, size_t size);

void* s21_realloc(void* block, size_t size);

void s21_free(void* block);

#ifdef __cplusplus
}
#endif
