#pragma once
#ifndef GPU_H_INCLUDED
#define GPU_H_INCLUDED

#include <dxgi.h>
#include <d3d12.h>
#include <stdbool.h>

#ifndef SWAP_CHAIN_BUFFER_COUNT
#define SWAP_CHAIN_BUFFER_COUNT 3
#endif

#ifndef DEFAULT_TEXTURE_HEAP_SIZE
#define DEFAULT_TEXTURE_HEAP_SIZE 16777216
#endif

#ifdef __cplusplus
extern "C" {
#endif

	typedef struct GPU_Framebuffer_
	{
		UINT8 colormask;

		struct
		{
			ID3D12Resource* color[8];
			ID3D12Resource* depthstencil;
		} resources;

		struct
		{
			D3D12_CPU_DESCRIPTOR_HANDLE srv[8];
			D3D12_CPU_DESCRIPTOR_HANDLE rtv[8];
			D3D12_CPU_DESCRIPTOR_HANDLE dsv;
		} handles;

	} GPU_Framebuffer;

	typedef struct GPU_RTV_Heap_
	{
		ID3D12DescriptorHeap*       d3dDescHeap;
		UINT64                      d3dMask;
		D3D12_CPU_DESCRIPTOR_HANDLE offset;
		UINT                        stride;
	} GPU_RTV_Heap;

	typedef struct GPU_DSV_Heap_
	{
		ID3D12DescriptorHeap*       d3dDescHeap;
		UINT64                      d3dMask;
		D3D12_CPU_DESCRIPTOR_HANDLE offset;
		UINT                        stride;
	} GPU_DSV_Heap;

	typedef struct GPU_TEX_Heap_
	{
		ID3D12Heap* d3dHeap;

	} GPU_TEX_Heap;

	typedef struct GPU_
	{
		IDXGIFactory1*      dxgiFactory;
		IDXGIAdapter1*      dxgiAdapter;
		IDXGISwapChain*     dxgiSwapChain;

		ID3D12Device*       d3dDevice;
		ID3D12CommandQueue* d3dGraphicsQueue;
		ID3D12CommandQueue* d3dCopyQueue;
		ID3D12CommandQueue* d3dComputeQueue;
		ID3D12Debug*        d3dDebug;

		GPU_RTV_Heap        d3dRtvHeap;
		GPU_DSV_Heap        d3dDsvHeap;
		GPU_TEX_Heap        d3dTexHeap;

		GPU_Framebuffer     framebuffer[SWAP_CHAIN_BUFFER_COUNT];
	} GPU;

	bool gpu_init(HWND hwnd_);
	void gpu_deinit();

	BOOL gpu_create_rtv_descritpor_handle(D3D12_CPU_DESCRIPTOR_HANDLE* handle_);
	void gpu_destroy_rtv_descriptor_handle(D3D12_CPU_DESCRIPTOR_HANDLE handle_);


	extern GPU gpudrv;

#ifdef __cplusplus
}
#endif

#endif
