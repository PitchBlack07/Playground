#include "gpu.h"
#include "gpu_desc_heap.h"
#include "utils.h"
#include <nmmintrin.h>
#include <assert.h>

static UINT get_desc_count(UINT64 mask_)
{
	const UINT* const u32 = (UINT*)&mask_;

	return _mm_popcnt_u32(u32[0]) + _mm_popcnt_u32(u32[1]);
}

static UINT get_desc_count(gpu_desc_heap* heap_)
{
	UINT v = 0;
	for (UINT i = 0; i < _countof(heap_->mask); ++i)
	{
		UINT* u32 = (UINT*)(heap_->mask + i);

		v += _mm_popcnt_u32(u32[0]);
		v += _mm_popcnt_u32(u32[1]);
	}
	return v;
}

BOOL gpu_desc_heap_init(gpu_desc_heap* heap_, D3D12_DESCRIPTOR_HEAP_TYPE type_)
{
	D3D12_DESCRIPTOR_HEAP_DESC dhd;
	dhd.Type           = type_;
	dhd.NumDescriptors = sizeof(heap_->mask) * 8;
	dhd.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dhd.NodeMask       = 0x00;

	if (SUCCEEDED(gpudrv.d3dDevice->CreateDescriptorHeap(&dhd, __uuidof(ID3D12DescriptorHeap), (void**)&heap_->heap))) {
		memset(heap_->mask, 0, sizeof(heap_->mask));

		heap_->offset = heap_->heap->GetCPUDescriptorHandleForHeapStart();
		heap_->stride = gpudrv.d3dDevice->GetDescriptorHandleIncrementSize(type_);

#ifdef _DEBUG
		switch (type_)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV: SET_DEBUG_NAME(heap_->heap, "RTV DESC Heap"); break;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV: SET_DEBUG_NAME(heap_->heap, "DSV DESC Heap"); break;
		default: SET_DEBUG_NAME(heap_->heap, "UNKNOWN DESC HEAP"); break;
		}
#endif

		return TRUE;
	}
	return false;
}

void gpu_desc_heap_deinit(gpu_desc_heap* heap_)
{
	assert(get_desc_count(heap_) == 0);

	SAFE_RELEASE(heap_->heap);
	memset(heap_, 0, sizeof(*heap_));
}

BOOL gpu_desc_heap_create_handle(gpu_desc_heap* heap_, D3D12_CPU_DESCRIPTOR_HANDLE* handleOut_)
{
	DWORD idx  = (DWORD)-1;
	UINT i     = 0;

	while (i < _countof(heap_->mask))
	{
		if (BitScanForward64(&idx, ~heap_->mask[i]))
		{
			break;
		}
		++i;
	}

	if (idx != (DWORD)-1)
	{
		const SIZE_T t  = i * sizeof(heap_->mask[0]) * 8 + (SIZE_T)idx;
		heap_->mask[i] |= (1ULL << idx);
		handleOut_->ptr = heap_->offset.ptr + t * heap_->stride;

		return TRUE;
	}
	return FALSE;
}

void gpu_desc_heap_destroy_handle(gpu_desc_heap* heap_, D3D12_CPU_DESCRIPTOR_HANDLE handle_)
{
	const SIZE_T bit        = (handle_.ptr - heap_->offset.ptr) / heap_->stride;
	const SIZE_T maskIdx    = bit / (sizeof(heap_->mask[0]) * 8);
	const SIZE_T bitMaskIdx = bit % (sizeof(heap_->mask[0]) * 8);
	const UINT64 clearMask  = (1ULL << bitMaskIdx);

	assert(heap_->mask[maskIdx] & clearMask);

	heap_->mask[maskIdx] ^= clearMask;
}