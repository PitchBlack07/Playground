#pragma once
#ifndef GPU_RESOURCE_BARRIER_H_INCLUDED
#define GPU_RESOURCE_BARRIER_H_INCLUDED

#include <d3d12.h>

struct gpu_resource_barrier
{
	D3D12_RESOURCE_BARRIER data[16];
	UINT                   size;
};

inline void gpu_resource_barrier_clear(gpu_resource_barrier* barrier_) {
	barrier_->size = 0;
}

void gpu_resource_barrier_commit(const gpu_resource_barrier* barrier_, ID3D12GraphicsCommandList* d3dCmdList);

#endif
