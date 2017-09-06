#pragma once
#ifndef GPU_DEPTH_STENCIL_BUFFER_H_INCLUDED
#define GPU_DEPTH_STENCIL_BUFFER_H_INCLUDED

#include "gpu_resource_barrier.h"

#include <d3d12.h>

struct gpu_depth_stencil_buffer
{
	ID3D12Resource*             resource;
	D3D12_CPU_DESCRIPTOR_HANDLE handle;
	D3D12_RESOURCE_STATES       state;
};

void gpu_depth_stencil_buffer_enable_write(gpu_depth_stencil_buffer* dsb, gpu_resource_barrier* barrier_);

#endif
