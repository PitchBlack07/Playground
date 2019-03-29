#pragma once
#include <stdint.h>
#include <stdbool.h>

typedef struct VktInitilizationInfo_
{
	struct {
		const char* const* Names;
		uint32_t           Count;
	} Extensions;

	struct {
		const char* const* Names;
		uint32_t           Count;
	} Layers;
} VktInitilizationInfo;

int32_t vktInit(const VktInitilizationInfo* info_);
void vktDeinit();

int32_t vktCreateWindow(uint32_t width_, uint32_t height_);
int32_t vktStartMessageLoop();