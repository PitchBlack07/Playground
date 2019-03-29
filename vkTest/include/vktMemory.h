#pragma once
#include <stdint.h>

#ifndef VKT_MEMORY_MACROS
#define VKT_MEMORY_MACROS

#define ALLOC_MEMORY(s__) vktalloc(s__, __FILE__, __LINE__)
#define ALLOC_POD(t__) vktalloc(sizeof(t__), __FILE__, __LINE__)
#define ALLOC_POD_ARRAY(t__, s__) vktalloc(sizeof(t__) * s__, __FILE__, __LINE__)
#define FREE_MEMORY(p__) vktfree(p__)
#endif

void* vktalloc(size_t size_, const char* file_, int line_);
void vktfree(void* ptr_);