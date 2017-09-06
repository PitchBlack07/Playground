#pragma once
#ifndef GPU_HEAP_H_INCLUDED
#define GPU_HEAP_H_INCLUDED

#include <d3d12.h>

typedef struct gpu_desc_heap_
{
	ID3D12DescriptorHeap*       heap;
	UINT64                      mask[4];
	D3D12_CPU_DESCRIPTOR_HANDLE offset;
	SIZE_T                      stride;
} gpu_desc_heap;

BOOL gpu_desc_heap_init(gpu_desc_heap* heap_, D3D12_DESCRIPTOR_HEAP_TYPE type_);
void gpu_desc_heap_deinit(gpu_desc_heap* heap_);

BOOL gpu_desc_heap_create_handle(gpu_desc_heap* heap_, D3D12_CPU_DESCRIPTOR_HANDLE* handleOut_);
void gpu_desc_heap_destroy_handle(gpu_desc_heap* heap_, D3D12_CPU_DESCRIPTOR_HANDLE handle_);

#endif
