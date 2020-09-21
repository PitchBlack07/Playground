#pragma once

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>

#include "error.h"

template<typename T>
void SafeRelease(T*& x) {
	if (x) {
		x->Release();
		x = nullptr;
	}
}

struct window
{
	ATOM ClassId;
	HWND Window;
	
	IDXGIFactory7*   DXGIFactory;
	IDXGIAdapter4*   DXGIAdapter;
	IDXGISwapChain3* DXGISwapChain;

	ID3D12Device*  D3D12Device;
	ID3D12CommandQueue* D3D12GraphicsCommandQueue;

	ID3D12CommandAllocator* D3D12CommandAllocator[3];
	ID3D12CommandList*      D3D12CommandList[3];
	ID3D12DescriptorHeap*   D3D12RTVHeap;
	ID3D12DescriptorHeap*   D3D12DSVHeap;
};

ErrorCode create_window(HINSTANCE, window* wndOut);

void show_window(window* wnd);
