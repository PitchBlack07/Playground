#include <assert.h>
#include "gpu_color_buffer.h"
#include "gpu.h"

static UINT gpu_color_buffer_transition(gpu_color_buffer* cb_, D3D12_RESOURCE_BARRIER* barrier_, UINT size_, D3D12_RESOURCE_STATES targetState_)
{
	UINT s      = 0;
	UINT mask   = cb_->mask;
	DWORD index = (DWORD)-1;

	while (_BitScanForward(&index, mask))
	{
		assert(s < size_);
		assert(cb_->resource[index]);

		if (cb_->states[index] != targetState_)
		{
			barrier_[s].Type                   = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
			barrier_[s].Flags                  = D3D12_RESOURCE_BARRIER_FLAG_NONE;
			barrier_[s].Transition.pResource   = cb_->resource[index];
			barrier_[s].Transition.Subresource = 0;
			barrier_[s].Transition.StateAfter  = targetState_;
			barrier_[s].Transition.StateBefore = cb_->states[index];
			cb_->states[index]                 = targetState_;

			s++;
		}

		mask ^= (1U << index);
	}
	return s;
}

static void gpu_color_buffer_transition(gpu_color_buffer* cb_, ID3D12GraphicsCommandList* d3dCmdList_, D3D12_RESOURCE_STATES targetState_)
{
	D3D12_RESOURCE_BARRIER b[_countof(cb_->handle)];

	const UINT s = gpu_color_buffer_transition(cb_, &b[0], _countof(b), targetState_);
	
	if (s)
	{
		d3dCmdList_->ResourceBarrier(s, b);
	}
}

void gpu_color_buffer_enable_write(gpu_color_buffer* cb_, gpu_resource_barrier* barrier_)
{
	D3D12_RESOURCE_BARRIER* barriers = barrier_->data + barrier_->size;
	const UINT freecount             = _countof(barrier_->data) - barrier_->size;
	const UINT barriersWritten       = gpu_color_buffer_transition(cb_, barriers, freecount, D3D12_RESOURCE_STATE_RENDER_TARGET);

	barrier_->size += barriersWritten;
}

void gpu_color_buffer_enable_present(gpu_color_buffer* cb_, gpu_resource_barrier* barrier_)
{
	D3D12_RESOURCE_BARRIER* barriers = barrier_->data + barrier_->size;
	const UINT freecount             = _countof(barrier_->data) - barrier_->size;
	const UINT barriersWritten       = gpu_color_buffer_transition(cb_, barriers, freecount, D3D12_RESOURCE_STATE_PRESENT);

	barrier_->size += barriersWritten;
}

void gpu_color_buffer_enable_write(gpu_color_buffer* cb_, ID3D12GraphicsCommandList* d3dCmdList_) 
{
	gpu_color_buffer_transition(cb_, d3dCmdList_, D3D12_RESOURCE_STATE_RENDER_TARGET);
}

void gpu_color_buffer_enable_present(gpu_color_buffer* cb_, ID3D12GraphicsCommandList* d3dCmdList_)
{
	gpu_color_buffer_transition(cb_, d3dCmdList_, D3D12_RESOURCE_STATE_PRESENT);
}