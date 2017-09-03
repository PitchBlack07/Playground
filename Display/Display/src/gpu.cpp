#pragma once
#include "utils.h"
#include "gpu.h"
#include "gpu_desc_heap.h"
#include <strsafe.h>
#include <d3dcommon.h>
#include <nmmintrin.h>
struct GPU_ gpudrv;

template<typename T>
static UINT get_bit_count()
{
	return sizeof(T) * 8;
}


static UINT count_active_bits(UINT* data_, UINT size_)
{
	UINT retval = 0;
	for (UINT i = 0; i <= size_; ++i)
	{
		retval += _mm_popcnt_u32(data_[i]);
	}
	return retval;
}

static UINT count_active_bits(UINT64 value_)
{
	const UINT* value = (UINT*)value_;
	return _mm_popcnt_u32(value[0]) + _mm_popcnt_u32(value[1]);
}

static bool adapter_init() 
{
	DXGI_ADAPTER_DESC1 desc;
	char buffer[256];

	if (FAILED(gpudrv.dxgiFactory->EnumAdapters1(0, &gpudrv.dxgiAdapter))) {
		return false;
	}

#ifdef _DEBUG
	gpudrv.dxgiAdapter->GetDesc1(&desc);

	StringCchPrintfA(buffer, sizeof(buffer), "Adapter [%s]", desc.Description);

	SET_DEBUG_NAME(gpudrv.dxgiAdapter, buffer);
#endif
	return true;
}

static bool d3d_device_init()
{
	if (SUCCEEDED(D3D12CreateDevice(gpudrv.dxgiAdapter, D3D_FEATURE_LEVEL_12_0, __uuidof(*gpudrv.d3dDevice), (void**)&gpudrv.d3dDevice))) {
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
	desc.Flags                              = 0;
	desc.OutputWindow                       = hwnd_;
	desc.SampleDesc.Count                   = 1;
	desc.SampleDesc.Quality                 = 0;
	desc.SwapEffect                         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc.Windowed                           = TRUE;

	return SUCCEEDED(gpudrv.dxgiFactory->CreateSwapChain(gpudrv.d3dGraphicsQueue, &desc, &gpudrv.dxgiSwapChain));
}

static bool gpu_desc_heap_init()
{
	D3D12_DESCRIPTOR_HEAP_DESC dhd;
	dhd.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	dhd.NumDescriptors = sizeof(gpudrv.d3dRtvHeap.d3dMask) * 8;
	dhd.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dhd.NodeMask = 0x00;

	if (FAILED(gpudrv.d3dDevice->CreateDescriptorHeap(&dhd, __uuidof(*gpudrv.d3dRtvHeap.d3dDescHeap), (void**)&gpudrv.d3dRtvHeap.d3dDescHeap))) {
		return false;
	}

	dhd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	if (FAILED(gpudrv.d3dDevice->CreateDescriptorHeap(&dhd, __uuidof(*gpudrv.d3dDsvHeap.d3dDescHeap), (void**)&gpudrv.d3dDsvHeap.d3dDescHeap))) {
		return false;
	}

	gpudrv.d3dRtvHeap.offset = gpudrv.d3dRtvHeap.d3dDescHeap->GetCPUDescriptorHandleForHeapStart();
	gpudrv.d3dRtvHeap.stride = gpudrv.d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	gpudrv.d3dDsvHeap.offset = gpudrv.d3dDsvHeap.d3dDescHeap->GetCPUDescriptorHandleForHeapStart();
	gpudrv.d3dDsvHeap.stride = gpudrv.d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);

	SET_DEBUG_NAME(gpudrv.d3dRtvHeap.d3dDescHeap, "Heap: DESC RTV");
	SET_DEBUG_NAME(gpudrv.d3dDsvHeap.d3dDescHeap, "Heap: DESC DSV");

	return true;
}

static void gpu_desc_heap_deinit()
{
	SAFE_RELEASE(gpudrv.d3dRtvHeap.d3dDescHeap);
	SAFE_RELEASE(gpudrv.d3dDsvHeap.d3dDescHeap);
}

static bool gpu_create_system_framebuffers()
{
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		if (FAILED(gpudrv.dxgiSwapChain->GetBuffer(0, __uuidof(ID3D12Resource), (void**)&gpudrv.framebuffer[i].resources.color[0]))) {
			return false;
		}

		D3D12_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.Format               = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		rtvd.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice   = 0;
		rtvd.Texture2D.PlaneSlice = 0;

		if (!gpu_create_rtv_descritpor_handle(&gpudrv.framebuffer[i].handles.rtv[0])) {
			return false;
		}

		gpudrv.d3dDevice->CreateRenderTargetView(gpudrv.framebuffer[i].resources.color[0], &rtvd, gpudrv.framebuffer[i].handles.rtv[0]);
	}
	return true;
}

static void gpu_destroy_system_framebuffers()
{
	for (UINT i = 0; i < SWAP_CHAIN_BUFFER_COUNT; ++i)
	{
		gpu_destroy_rtv_descriptor_handle(gpudrv.framebuffer[i].handles.rtv[i]);
		SAFE_RELEASE(gpudrv.framebuffer[i].resources.color[0]);

		gpudrv.framebuffer[i].handles.rtv[i] = {};
	}
}


static bool gpu_tex_heap_init()
{
	D3D12_HEAP_DESC hd;
	hd.SizeInBytes                = DEFAULT_TEXTURE_HEAP_SIZE;
	hd.Properties.Type            = D3D12_HEAP_TYPE_DEFAULT;
	hd.Properties.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
	hd.Properties.CreationNodeMask = 0x00;
	hd.Properties.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
	hd.Properties.VisibleNodeMask = 0x00;
	hd.Alignment = D3D12_DEFAULT_RESOURCE_PLACEMENT_ALIGNMENT;
	hd.Flags = D3D12_HEAP_FLAG_ALLOW_ALL_BUFFERS_AND_TEXTURES;
	
	
	if (FAILED(gpudrv.d3dDevice->CreateHeap(&hd, __uuidof(*gpudrv.d3dTexHeap.d3dHeap), (void**)&gpudrv.d3dTexHeap.d3dHeap))) {
		return false;
	}

	SET_DEBUG_NAME(gpudrv.d3dTexHeap.d3dHeap, "Texture Heap");

	return true;
}

static void gpu_tex_heap_deinit()
{
	SAFE_RELEASE(gpudrv.d3dTexHeap.d3dHeap);
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

	if (!gpu_desc_heap_init()) {
		return false;
	}

	if (!gpu_tex_heap_init()) {
		return false;
	}

	if (!gpu_create_system_framebuffers()) {
		return false;
	}

	return true;
}

void gpu_deinit()
{
	gpu_destroy_system_framebuffers();
	gpu_tex_heap_deinit();
	gpu_desc_heap_deinit();

	SAFE_RELEASE(gpudrv.dxgiSwapChain);
	SAFE_RELEASE(gpudrv.d3dCopyQueue);
	SAFE_RELEASE(gpudrv.d3dComputeQueue);
	SAFE_RELEASE(gpudrv.d3dGraphicsQueue);

	SAFE_RELEASE(gpudrv.d3dDevice);
	SAFE_RELEASE(gpudrv.dxgiAdapter);
	SAFE_RELEASE(gpudrv.dxgiFactory);
	SAFE_RELEASE(gpudrv.d3dDebug);
}

static BOOL get_free_index(UINT64 mask_, UINT* index_)
{
	DWORD index;
	if (BitScanForward64(&index, ~mask_))
	{
		*index_ = (UINT)index;
		return TRUE;
	}
	return FALSE;
}

BOOL gpu_create_rtv_descritpor_handle(D3D12_CPU_DESCRIPTOR_HANDLE* handle_)
{
	DWORD index;
	if (BitScanForward64(&index, ~gpudrv.d3dRtvHeap.d3dMask))
	{
		gpudrv.d3dRtvHeap.d3dMask |= 1ULL << index;

		handle_->ptr = gpudrv.d3dRtvHeap.offset.ptr + index * gpudrv.d3dRtvHeap.stride;

		return true;
	}
	return false;
}

void gpu_destroy_rtv_descriptor_handle(D3D12_CPU_DESCRIPTOR_HANDLE handle_)
{
	DWORD index                = (handle_.ptr - gpudrv.d3dRtvHeap.offset.ptr) / gpudrv.d3dRtvHeap.stride;
	UINT64 mask                = 1ULL << index;
	gpudrv.d3dRtvHeap.d3dMask &= ~mask;
}
