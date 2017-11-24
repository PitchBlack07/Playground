#include "gpu_ring_buffer.h"
#include "gpu.h"
#include "utils.h"
#include <assert.h>

static const UINT RingBufferSize = 1024 * 1024;

static UINT Align64kb(UINT size_)
{
	static const UINT alignment = 64 * 1024;

	return (size_ + alignment - 1) & ~(alignment - 1);
}

BOOL gpu_ring_buffer_init(gpu_ring_buffer* rb_)
{
	memset(rb_, 0, sizeof(*rb_));

	D3D12_HEAP_PROPERTIES hp;
	hp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.CreationNodeMask     = 0x00;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.Type                 = D3D12_HEAP_TYPE_UPLOAD;
	hp.VisibleNodeMask      = 0x00;

	D3D12_RESOURCE_DESC rd;
	rd.Dimension          = D3D12_RESOURCE_DIMENSION_BUFFER;
	rd.Alignment          = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	rd.Width              = RingBufferSize;
	rd.Height             = 1;
	rd.DepthOrArraySize   = 1;
	rd.MipLevels          = 1;
	rd.Format             = DXGI_FORMAT_UNKNOWN;
	rd.SampleDesc.Count   = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout             = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	rd.Flags              = D3D12_RESOURCE_FLAG_NONE;

	HRESULT hr = gpudrv.d3dDevice->CreateCommittedResource(
		&hp, 
		D3D12_HEAP_FLAG_NONE, 
		&rd, 
		D3D12_RESOURCE_STATE_GENERIC_READ, 
		NULL, 
		__uuidof(ID3D12Resource), 
		(void**)&rb_->buffer);

	if (SUCCEEDED(hr)) {
		rb_->free      = RingBufferSize;
		rb_->head      = 0;
		rb_->tail      = 0;
		rb_->delq.size = 0;

		return TRUE;
	}

	return FALSE;
}

void gpu_ring_buffer_deinit(gpu_ring_buffer* rb_)
{
	SAFE_RELEASE(rb_->buffer);
	memset(rb_, 0, sizeof(*rb_));
}

void gpu_ring_buffer_release_memory(gpu_ring_buffer* rb_, UINT64 frameId_)
{
	gpu_ring_buffer_deletion_queue* const q = &rb_->delq;
	UINT64 ptr = q->ptr;
	UINT size  = q->size;

	while (size && q->buffer[ptr].frameId <= frameId_)
	{
		rb_->tail = (rb_->tail + q->buffer[ptr].size) % RingBufferSize;
		ptr       = (ptr + 1) % _countof(q->buffer);

		size--;
	}
}

void delq_append(gpu_ring_buffer_deletion_queue* q_, UINT size_, UINT64 frameId_)
{
	if (q_->size < _countof(q_->buffer))
	{
		q_->buffer[q_->ptr].frameId = frameId_;
		q_->buffer[q_->ptr].size    = size_;
		q_->ptr                     = (q_->ptr + 1) % _countof(q_->buffer);
		q_->size++;
	}
}

D3D12_GPU_VIRTUAL_ADDRESS gpu_ring_buffer_alloc(gpu_ring_buffer* rb_, UINT size_, UINT64 frameId_)
{
	UINT alignedsize = Align64kb(size_);

	UINT tail = rb_->tail;
	UINT head = rb_->head;
	UINT free = rb_->free;

	D3D12_GPU_VIRTUAL_ADDRESS base = rb_->buffer->GetGPUVirtualAddress();

	if (free < size_) {
		return (D3D12_GPU_VIRTUAL_ADDRESS)0;
	}

	if (tail < head) {
		if (head + alignedsize < RingBufferSize) {

			UINT64 offset = rb_->head;
			rb_->head    += alignedsize;
			rb_->free    -= alignedsize;

			delq_append(&rb_->delq, alignedsize, frameId_);

			return base + head;
		}
		else if (alignedsize <= tail) {
			UINT64 offset = 0;
			UINT tmp      = alignedsize + RingBufferSize - head;
			rb_->free    -= tmp;
			rb_->head     = alignedsize;

			delq_append(&rb_->delq, tmp, frameId_);

			return base + offset;
		}
		else {
			return 0;
		}
	}
	else if (head + alignedsize <= tail) {
		UINT64 offset = rb_->head;
		rb_->free    -= alignedsize;
		rb_->head    += alignedsize;

		delq_append(&rb_->delq, alignedsize, frameId_);

		return base + offset;
	}
	else {
		assert(rb_->free == RingBufferSize);

		rb_->head = alignedsize;
		rb_->tail = 0;
		rb_->free = RingBufferSize - alignedsize;

		delq_append(&rb_->delq, alignedsize, frameId_);

		return base;
	}
}