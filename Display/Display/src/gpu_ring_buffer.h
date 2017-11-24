#pragma once
#ifndef GPU_RING_BUFFER_H_INCLUDED
#define GPU_RING_BUFFER_H_INCLUDED

#include "gpu.h"
#include <d3d12.h>

struct gpu_ring_buffer_allocation
{
	UINT64 frameId;
	UINT   size;
};

struct gpu_ring_buffer_deletion_queue
{
	gpu_ring_buffer_allocation buffer[32];
	UINT                       size;
	UINT                       ptr;
};

struct gpu_ring_buffer
{
	ID3D12Resource* buffer;
	UINT            head;
	UINT            tail;
	UINT            free;
	UINT            bufferSizeInBytes;

	gpu_ring_buffer_deletion_queue delq;
};

BOOL gpu_ring_buffer_init(gpu_ring_buffer* rb_);
void gpu_ring_buffer_deinit(gpu_ring_buffer* rb_);
void gpu_ring_buffer_release_memory(gpu_ring_buffer* rb_, UINT64 frameId_);
D3D12_GPU_VIRTUAL_ADDRESS gpu_ring_buffer_alloc(gpu_ring_buffer* rb_, UINT size_, UINT64 frameId_);

#endif