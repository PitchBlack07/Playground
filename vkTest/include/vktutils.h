#pragma once
#include <stdint.h>

uint32_t SelectSupportedExtensions(const char* const* requrested_, const char** supported_, uint32_t size_);
uint32_t SelectSupportedLayers(const char* const* requested_, const char** supported_, uint32_t size_);
