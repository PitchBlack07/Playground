#pragma once
#include "utils.h"
#include "display.h"
#include "gpu.h"
#include "gpu_desc_heap.h"
#include <strsafe.h>
#include <d3dcommon.h>
#include <nmmintrin.h>

//
// Global variable
//
GPU gpudrv;

static const TCHAR* ClassName = TEXT("gpu_window_class");
static LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);

static BOOL gpu_create_window(HINSTANCE hInstance_, UINT width_, UINT height_)
{
	WNDCLASS wc;

	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_BACKGROUND);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance     = hInstance_;
	wc.lpfnWndProc   = &WndProc;
	wc.lpszClassName = ClassName;
	wc.lpszMenuName  = nullptr;
	wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

	if (!RegisterClass(&wc)) {
		return FALSE;
	}

	gpudrv.hInstance = hInstance_;
	gpudrv.hwnd      = CreateWindow(ClassName, TEXT("Display"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (INT)width_, (INT)height_, nullptr, nullptr, hInstance_, nullptr);

	if (gpudrv.hwnd == NULL) {
		return FALSE;
	}

	ShowWindow(gpudrv.hwnd, SW_SHOW);

	return TRUE;
}



static void gpu_wait_for_completion()
{
	ID3D12Fence* fence = nullptr;

	if (FAILED(gpudrv.d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence))) {
		return;
	}

	gpudrv.d3dGraphicsQueue->Signal(fence, 1);

	if (fence->GetCompletedValue() < 1) {
		fence->SetEventOnCompletion(1, gpudrv.d3dFenceEvent);

		WaitForSingleObject(gpudrv.d3dFenceEvent, INFINITE);
	}

	SAFE_RELEASE(fence);
}

static bool gpu_create_system_color_buffers()
{
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		if (FAILED(gpudrv.dxgiSwapChain->GetBuffer(
			i,
			__uuidof(ID3D12Resource),
			(void**)&gpudrv.sysColorBuffers[i].resource[0])))
		{
			return false;
		}

		if (!gpu_desc_heap_create_handle(&gpudrv.d3dRtvHeap, &gpudrv.sysColorBuffers[i].handle[0])) {
			return false;
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvd.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		rtvd.Texture2D.PlaneSlice = 0;

		gpudrv.d3dDevice->CreateRenderTargetView(
			gpudrv.sysColorBuffers[i].resource[0],
			&rtvd,
			gpudrv.sysColorBuffers[i].handle[0]);

		gpudrv.sysColorBuffers[i].mask = 0x1;
		gpudrv.sysColorBuffers[i].states[0] = D3D12_RESOURCE_STATE_COMMON;
	}

	return true;
	//D3D12_HEAP_PROPERTIES hp;
	//hp.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	//hp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	//hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	//hp.CreationNodeMask     = 0x00;
	//hp.VisibleNodeMask      = 0x00;


	//D3D12_RESOURCE_DESC rd;
	//rd.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	//rd.Alignment          = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	//rd.Width              = colordesc.Width;
	//rd.Height             = colordesc.Height;
	//rd.DepthOrArraySize   = 1;
	//rd.MipLevels          = 1;
	//rd.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	//rd.SampleDesc.Count   = 1;
	//rd.SampleDesc.Quality = 0;
	//rd.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	//rd.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	//if (SUCCEEDED(gpudrv.d3dDevice->CreateCommittedResource(
	//	&hp, 
	//	D3D12_HEAP_FLAG_NONE, 
	//	&rd, 
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE, 
	//	NULL, 
	//	__uuidof(ID3D12Resource), 
	//	(void**)&gpudrv.framebuffer[0].resources.depthstencil))) 
	//{
	//	gpudrv.framebuffer[1].resources.depthstencil = gpudrv.framebuffer[0].resources.depthstencil;
	//	gpudrv.framebuffer[2].resources.depthstencil = gpudrv.framebuffer[0].resources.depthstencil;

	//	gpudrv.framebuffer[0].resources.depthstencil->AddRef();
	//	gpudrv.framebuffer[0].resources.depthstencil->AddRef();

	//	return true;
	//}
	//return false;
}

static void gpu_destroy_system_color_buffers()
{
	for (UINT i = 0; i < _countof(gpudrv.sysColorBuffers); ++i)
	{
		gpu_desc_heap_destroy_handle(
			&gpudrv.d3dRtvHeap,
			gpudrv.sysColorBuffers[i].handle[0]);

		SAFE_RELEASE(gpudrv.sysColorBuffers[i].resource[0]);
	}
}

static bool gpu_create_system_depth_stencil_buffer()
{

	DXGI_SWAP_CHAIN_DESC scd;
	gpudrv.dxgiSwapChain->GetDesc(&scd);

	D3D12_HEAP_PROPERTIES hp;
	hp.Type                 = D3D12_HEAP_TYPE_DEFAULT;
	hp.CPUPageProperty      = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hp.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hp.CreationNodeMask     = 0x00;
	hp.VisibleNodeMask      = 0x00;


	D3D12_RESOURCE_DESC rd;
	rd.Dimension          = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	rd.Alignment          = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	rd.Width              = (UINT)scd.BufferDesc.Width;
	rd.Height             = (UINT)scd.BufferDesc.Height;
	rd.DepthOrArraySize   = 1;
	rd.MipLevels          = 1;
	rd.Format             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	rd.SampleDesc.Count   = 1;
	rd.SampleDesc.Quality = 0;
	rd.Layout             = D3D12_TEXTURE_LAYOUT_UNKNOWN;
	rd.Flags              = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	D3D12_CLEAR_VALUE cv    = {};
	cv.DepthStencil.Depth   = 1.f;
	cv.DepthStencil.Stencil = 0;
	cv.Format               = rd.Format;

	if (SUCCEEDED(gpudrv.d3dDevice->CreateCommittedResource(
		&hp, 
		D3D12_HEAP_FLAG_NONE, 
		&rd, 
		D3D12_RESOURCE_STATE_DEPTH_WRITE, 
		&cv, 
		__uuidof(ID3D12Resource), 
		(void**)&gpudrv.sysDepthStencilBuffer.resource)))
	{
		gpudrv.sysDepthStencilBuffer.state = D3D12_RESOURCE_STATE_DEPTH_WRITE;
		
		if (gpu_desc_heap_create_handle(&gpudrv.d3dDsvHeap, &gpudrv.sysDepthStencilBuffer.handle))
		{
			D3D12_DEPTH_STENCIL_VIEW_DESC dsvd;
			dsvd.Flags              = D3D12_DSV_FLAG_NONE;
			dsvd.Format             = rd.Format;
			dsvd.Texture2D.MipSlice = 0;
			dsvd.ViewDimension      = D3D12_DSV_DIMENSION_TEXTURE2D;

			gpudrv.d3dDevice->CreateDepthStencilView(gpudrv.sysDepthStencilBuffer.resource, &dsvd, gpudrv.sysDepthStencilBuffer.handle);
			return true;
		}

		SAFE_RELEASE(gpudrv.sysDepthStencilBuffer.resource);
	}
	return false;
}

static void gpu_destroy_system_depth_stencil_buffer()
{
	gpu_desc_heap_destroy_handle(&gpudrv.d3dDsvHeap, gpudrv.sysDepthStencilBuffer.handle);
	SAFE_RELEASE(gpudrv.sysDepthStencilBuffer.resource);
}

static bool adapter_init() 
{
	if (FAILED(gpudrv.dxgiFactory->EnumAdapters1(0, &gpudrv.dxgiAdapter))) {
		return false;
	}

#ifdef _DEBUG
	char buffer[256];
	DXGI_ADAPTER_DESC1 desc;
	gpudrv.dxgiAdapter->GetDesc1(&desc);

	StringCchPrintfA(buffer, sizeof(buffer), "Adapter [%s]", desc.Description);

	SET_DEBUG_NAME(gpudrv.dxgiAdapter, buffer);
#endif
	return true;
}

static bool d3d_device_init()
{
	if (SUCCEEDED(D3D12CreateDevice(gpudrv.dxgiAdapter, D3D_FEATURE_LEVEL_11_0, __uuidof(*gpudrv.d3dDevice), (void**)&gpudrv.d3dDevice))) {
		SET_DEBUG_NAME(gpudrv.d3dDevice, "D3D Device");
		return true;
	}

	return false;
}

static bool d3d_command_queues_init()
{
	D3D12_COMMAND_QUEUE_DESC desc;
	desc.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0x00;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;

	if (FAILED(gpudrv.d3dDevice->CreateCommandQueue(&desc, __uuidof(gpudrv.d3dGraphicsQueue), (void**)&gpudrv.d3dGraphicsQueue))) {
		return false;
	}

	desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

	if (FAILED(gpudrv.d3dDevice->CreateCommandQueue(&desc, __uuidof(gpudrv.d3dComputeQueue), (void**)&gpudrv.d3dComputeQueue))) {
		return false;
	}

	desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	if (FAILED(gpudrv.d3dDevice->CreateCommandQueue(&desc, __uuidof(gpudrv.d3dCopyQueue), (void**)&gpudrv.d3dCopyQueue))) {
		return false;
	}

	SET_DEBUG_NAME(gpudrv.d3dGraphicsQueue, "Graphics Queue");
	SET_DEBUG_NAME(gpudrv.d3dComputeQueue, "Compute Queue");
	SET_DEBUG_NAME(gpudrv.d3dCopyQueue, "Copy Queue");

	return true;
}

static bool dxgi_swap_chain_init(HWND hwnd_)
{
	RECT                 rect;
	DXGI_SWAP_CHAIN_DESC desc;


	if (!GetClientRect(hwnd_, &rect)) {
		return false;
	}

	desc.BufferCount                        = SWAP_CHAIN_BUFFER_COUNT;
	desc.BufferUsage                        = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc.BufferDesc.Format                  = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc.BufferDesc.Height                  = (UINT)(rect.right - rect.left);
	desc.BufferDesc.Width                   = (UINT)(rect.bottom - rect.top);
	desc.BufferDesc.RefreshRate.Denominator = 0;
	desc.BufferDesc.RefreshRate.Numerator   = 0;
	desc.BufferDesc.Scaling                 = DXGI_MODE_SCALING_UNSPECIFIED;
	desc.BufferDesc.ScanlineOrdering        = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	desc.Flags                              = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
	desc.OutputWindow                       = hwnd_;
	desc.SampleDesc.Count                   = 1;
	desc.SampleDesc.Quality                 = 0;
	desc.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Windowed                           = TRUE;

	IDXGISwapChain* sc = NULL;
	if (FAILED(gpudrv.dxgiFactory->CreateSwapChain(gpudrv.d3dGraphicsQueue, &desc, &sc))) {
		return false;
	}

	HRESULT hr = sc->QueryInterface(__uuidof(*gpudrv.dxgiSwapChain), (void**)&gpudrv.dxgiSwapChain);
	SAFE_RELEASE(sc);

	if (FAILED(hr)) {
		return false;
	}

	gpudrv.dxgiSwapChain->SetMaximumFrameLatency(SWAP_CHAIN_BUFFER_COUNT);

	return true;
}

static bool gpu_command_allocators_create()
{
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		if (FAILED(gpudrv.d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&gpudrv.d3dCommandAllocator[i]))) {
			return false;
		}
	}
	return true;
}

static void gpu_command_allocators_destroy()
{
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		SAFE_RELEASE(gpudrv.d3dCommandAllocator[i]);
	}
}

static bool gpu_create_fence()
{
	if (FAILED(gpudrv.d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&gpudrv.d3dFence))) {
		return false;
	}

	gpudrv.d3dFenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

	return gpudrv.d3dFenceEvent != NULL;
}

bool gpu_init(HWND hwnd_)
{
	memset(&gpudrv, 0, sizeof(gpudrv));

#ifdef _DEBUG
	if (FAILED(D3D12GetDebugInterface(__uuidof(*gpudrv.d3dDebug), (void**)&gpudrv.d3dDebug))) {
		return false;
	}

	gpudrv.d3dDebug->EnableDebugLayer();
#endif

	if (FAILED(CreateDXGIFactory1(__uuidof(gpudrv.dxgiFactory), (void**)&gpudrv.dxgiFactory))) {
		return false;
	}

	if (!adapter_init()) {
		return false;
	}

	if (!d3d_device_init()) {
		return false;
	}

	if (!d3d_command_queues_init()) {
		return false;
	}

	if (!dxgi_swap_chain_init(hwnd_)) {
		return false;
	}

	if (!gpu_desc_heap_init(&gpudrv.d3dRtvHeap, D3D12_DESCRIPTOR_HEAP_TYPE_RTV)) {
		return false;
	}

	if (!gpu_desc_heap_init(&gpudrv.d3dDsvHeap, D3D12_DESCRIPTOR_HEAP_TYPE_DSV)) {
		return false;
	}

	if (!gpu_command_allocators_create()) {
		return false;
	}

	if (!gpu_create_system_color_buffers()) {
		return false;
	}

	if (!gpu_create_system_depth_stencil_buffer()) {
		return false;
	}

	if (!gpu_create_fence()) {
		return false;
	}

	return true;
}


static void gpu_destroy_fence()
{
	CloseHandle(gpudrv.d3dFenceEvent);
	SAFE_RELEASE(gpudrv.d3dFence);
}

void gpu_deinit()
{
	gpu_wait_for_completion();

	gpu_destroy_fence();
	gpu_destroy_system_depth_stencil_buffer();
	gpu_destroy_system_color_buffers();
	gpu_command_allocators_destroy();
	
	gpu_desc_heap_deinit(&gpudrv.d3dDsvHeap);
	gpu_desc_heap_deinit(&gpudrv.d3dRtvHeap);

	SAFE_RELEASE(gpudrv.dxgiSwapChain);
	SAFE_RELEASE(gpudrv.d3dCopyQueue);
	SAFE_RELEASE(gpudrv.d3dComputeQueue);
	SAFE_RELEASE(gpudrv.d3dGraphicsQueue);

	SAFE_RELEASE(gpudrv.d3dDevice);
	SAFE_RELEASE(gpudrv.dxgiAdapter);
	SAFE_RELEASE(gpudrv.dxgiFactory);
	SAFE_RELEASE(gpudrv.d3dDebug);
}

BOOL gpu_begin_frame()
{
	if (gpudrv.dxgiSwapChain == NULL)
	{
		return FALSE;
	}

	gpudrv.frameId++;

	HANDLE swaplock = gpudrv.dxgiSwapChain->GetFrameLatencyWaitableObject();
	WaitForSingleObject(swaplock, INFINITE);

	const UINT64 completedValue = gpudrv.d3dFence->GetCompletedValue();

	if (completedValue + SWAP_CHAIN_BUFFER_COUNT < gpudrv.frameId) {

		gpudrv.d3dFence->SetEventOnCompletion(gpudrv.frameId - SWAP_CHAIN_BUFFER_COUNT, gpudrv.d3dFenceEvent);

		WaitForSingleObject(gpudrv.d3dFence, INFINITE);
	}
	
	return TRUE;
}

void gpu_end_frame()
{
	gpudrv.d3dGraphicsQueue->Signal(gpudrv.d3dFence, gpudrv.frameId);
	gpudrv.dxgiSwapChain->Present(0, 0);
}

ID3D12CommandAllocator* gpu_get_command_allocator()
{
	return gpudrv.d3dCommandAllocator[gpudrv.frameId % SWAP_CHAIN_BUFFER_COUNT];
}

BOOL gpu_start(HINSTANCE hInstance, UINT width_, UINT height_)
{
	return gpu_create_window(hInstance, width_, height_);
}

void gpu_stop()
{
	UnregisterClass(ClassName, gpudrv.hInstance);
}

static LRESULT WM_CLOSE_Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	gpu_deinit();
	DestroyWindow(hwnd_);
	return 0;
}

static LRESULT WM_DESTROY_Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	PostQuitMessage(0);
	return 0;
}

static LRESULT WM_CREATE_Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	return gpu_init(hwnd_) ? 0 : -1;
}

static LRESULT WM_SIZE_Handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	UINT width = (UINT)LOWORD(lparam_);
	UINT height = (UINT)HIWORD(lparam_);

	gpu_wait_for_completion();
	gpu_destroy_system_color_buffers();
	gpu_destroy_system_depth_stencil_buffer();

	DXGI_SWAP_CHAIN_DESC desc;
	gpudrv.dxgiSwapChain->GetDesc(&desc);

	if (FAILED(gpudrv.dxgiSwapChain->ResizeBuffers(SWAP_CHAIN_BUFFER_COUNT, width, height, desc.BufferDesc.Format, desc.Flags))) {
		return -1;
	}

	if (!gpu_create_system_color_buffers()) {
		return -1;
	}

	if (!gpu_create_system_depth_stencil_buffer()) {
		return -1;
	}

	return 0;
}

#define IN_CASE_HANDLE(x) case x: return x##_Handler(hwnd_, msg_, wparam_, lparam_)

static LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch (msg_)
	{
	IN_CASE_HANDLE(WM_CREATE);
	IN_CASE_HANDLE(WM_SIZE);
	IN_CASE_HANDLE(WM_CLOSE);
	IN_CASE_HANDLE(WM_DESTROY);

	default:
		return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
	}
}