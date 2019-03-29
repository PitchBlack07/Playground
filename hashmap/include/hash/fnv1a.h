#pragma once
#ifndef HASHMAP_HASH_FUNCTIONS
#define HASHMAP_HASH_FUNCTIONS

#include <stdint.h>

#define FNV_OFFSET_BIAS ((uint32_t)0x811C9DC5)

uint32_t fnv1a32(const void* data_, uint32_t dataSizeInBytes_, uint32_t offset_);

#endif
