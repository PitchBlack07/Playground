#pragma once
#ifndef GPU_RING_BUFFER_H_INCLUDED
#define GPU_RING_BUFFER_H_INCLUDED

#include "gpu.h"
#include <d3d12.h>

struct gpu_ring_buffer_allocation
{
	UINT64 frameId;
	UINT   ptr;
};

struct gpu_ring_buffer_deletion_queue
{
	gpu_ring_buffer_allocation allocations[32];
	UINT                       size;
};

struct gpu_ring_buffer
{
	ID3D12Resource* buffer;
	UINT            head;
	UINT            tail;
	UINT            size;

	gpu_ring_buffer_deletion_queue delq;
};

#endif