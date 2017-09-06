#pragma once
#include "gpu_resource_barrier.h"

void gpu_resource_barrier_commit(const gpu_resource_barrier* barrier_, ID3D12GraphicsCommandList* d3dCmdList)
{
	if (barrier_->size)
	{
		d3dCmdList->ResourceBarrier(barrier_->size, barrier_->data);
	}
}
