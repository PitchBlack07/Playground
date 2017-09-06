#pragma once
#include "gpu_depth_stencil_buffer.h"

static UINT gpu_depth_stencil_buffer_transition(gpu_depth_stencil_buffer* dsb_, D3D12_RESOURCE_BARRIER* barrier_, UINT size_, D3D12_RESOURCE_STATES targetState_)
{
	if (dsb_->state != targetState_)
	{
		barrier_->Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		barrier_->Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		barrier_->Transition.pResource   = dsb_->resource;
		barrier_->Transition.Subresource = 0;
		barrier_->Transition.StateAfter  = targetState_;
		barrier_->Transition.StateBefore = dsb_->state;
		dsb_->state                      = targetState_;

		return 1;
	}
	return 0;
}

void gpu_depth_stencil_buffer_enable_write(gpu_depth_stencil_buffer* dsb_, gpu_resource_barrier* barrier_)
{
	barrier_->size += gpu_depth_stencil_buffer_transition(
		dsb_, 
		barrier_->data + barrier_->size, 
		_countof(barrier_->data) - barrier_->size, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE);
}
