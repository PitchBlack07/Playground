#pragma once
#ifndef GPU_H_INCLUDED
#define GPU_H_INCLUDED

#include <dxgi1_6.h>
#include <d3d12.h>
#include <stdbool.h>
#include "gpu_fb.h"
#include "gpu_desc_heap.h"
#include "gpu_color_buffer.h"
#include "gpu_depth_stencil_buffer.h"

#ifndef SWAP_CHAIN_BUFFER_COUNT
#define SWAP_CHAIN_BUFFER_COUNT 3
#endif

#ifndef DEFAULT_TEXTURE_HEAP_SIZE
#define DEFAULT_TEXTURE_HEAP_SIZE 16777216
#endif

struct GPU
{
	HWND                     hwnd;
	HINSTANCE                hInstance;

	IDXGIFactory1*           dxgiFactory;
	IDXGIAdapter1*           dxgiAdapter;
	IDXGISwapChain3*         dxgiSwapChain;
							     
	ID3D12Device*            d3dDevice;
	ID3D12CommandQueue*      d3dGraphicsQueue;
	ID3D12CommandQueue*      d3dCopyQueue;
	ID3D12CommandQueue*      d3dComputeQueue;
	ID3D12CommandAllocator*  d3dCommandAllocator[SWAP_CHAIN_BUFFER_COUNT];
	ID3D12Debug*             d3dDebug;
	ID3D12Fence*             d3dFence;
	HANDLE                   d3dFenceEvent;

	gpu_desc_heap            d3dRtvHeap;
	gpu_desc_heap            d3dDsvHeap;
	gpu_color_buffer         sysColorBuffers[SWAP_CHAIN_BUFFER_COUNT];
	gpu_depth_stencil_buffer sysDepthStencilBuffer;

	UINT64                   frameId;
};


BOOL gpu_start(HINSTANCE hInstance, UINT width_, UINT height_);
void gpu_stop();

BOOL gpu_begin_frame();
void gpu_end_frame();

ID3D12CommandAllocator* gpu_get_command_allocator();
extern GPU gpudrv;

inline ID3D12Device* gpu_get_device() {
	return gpudrv.d3dDevice;
}

inline ID3D12CommandQueue* gpu_get_command_queue() {
	return gpudrv.d3dGraphicsQueue;
}

inline gpu_color_buffer* gpu_get_system_color_buffer() {
	return &gpudrv.sysColorBuffers[gpudrv.dxgiSwapChain->GetCurrentBackBufferIndex()];
}

inline gpu_depth_stencil_buffer* gpu_get_system_depth_stencil_buffer() {
	return &gpudrv.sysDepthStencilBuffer;
}

#endif
