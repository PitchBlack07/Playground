#pragma once
#ifndef GPU_COLOR_BUFFER_H_INCLUDED
#define GPU_COLOR_BUFFER_H_INCLUDED

#include "gpu_resource_barrier.h"

#include <d3d12.h>

struct gpu_color_buffer
{
	UINT8                       mask;
	ID3D12Resource*             resource[8];
	D3D12_CPU_DESCRIPTOR_HANDLE handle[8];
	D3D12_RESOURCE_STATES       states[8];
};

void gpu_color_buffer_enable_write(gpu_color_buffer* cb_, gpu_resource_barrier* barrier_);
void gpu_color_buffer_enable_present(gpu_color_buffer* cb_, gpu_resource_barrier* barrier_);

void gpu_color_buffer_enable_write(gpu_color_buffer* cb_, ID3D12GraphicsCommandList* d3dCmdList_);
void gpu_color_buffer_enable_present(gpu_color_buffer* cb_, ID3D12GraphicsCommandList* d3dCmdList_);


#endif
