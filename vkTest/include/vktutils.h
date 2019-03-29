#pragma once
#include <stdint.h>

typedef struct VktVersion_ {
	uint8_t  Major;
	uint8_t  Minor;
	uint16_t Build;
} VktVersion;

VktVersion vktGetVersion();

uint32_t vktSelectSupportedExtensions(const char* const* requrested_, const char** supported_, uint32_t size_);
uint32_t vktSelectSupportedLayers(const char* const* requested_, const char** supported_, uint32_t size_);

void vktPrintAvailableLayers();
void vkPrintAvaiableExtensions();