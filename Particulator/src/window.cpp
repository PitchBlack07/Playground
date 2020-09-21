#include "window.h"
#include <dxgidebug.h>
#include <dxgi1_6.h>
#include <d3d12.h>
#include <d3dcommon.h>
#include <stdio.h>
#include <assert.h>

#ifndef SET_DEBUG_NAME
#define SET_DEBUG_NAME(x, name) SetDebugInfo(x, name, __FILE__, __LINE__)
#endif

template<typename T>
static void SetDebugInfo(T* obj_, const char* text, const char* file, int line)
{
	char buffer[512];

	if (text) {
		sprintf_s(buffer, sizeof(buffer), "%s@%s:%i", text, file, line);
	}
	else {
		sprintf_s(buffer, sizeof(buffer), "%s:%i", file, line);
	}

	obj_->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(buffer), buffer);
}

static void clear_window(window* w) {
	w->ClassId = 0;
	w->Window  = nullptr;
}

static ErrorCode init_dxgi(window* w) {
	HRESULT hr = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG , IID_PPV_ARGS(&w->DXGIFactory));
	
	if (FAILED(hr)) {
		return ErrorCode::DXGI_ERROR;
	}

	SET_DEBUG_NAME(w->DXGIFactory, "DXGIFactory");

	IDXGIAdapter1* adapter1 = nullptr;
	hr = w->DXGIFactory->EnumAdapters1(0, &adapter1);
	if (FAILED(hr)) {
		return ErrorCode::DXGI_ADAPTER_ERROR;
	}

	hr = adapter1->QueryInterface<IDXGIAdapter4>(&w->DXGIAdapter);
	SafeRelease(adapter1);

	if (FAILED(hr)) {
		return ErrorCode::DXGI_ADAPTER_ERROR;
	}

	return ErrorCode::SUCCESS;
}

static ErrorCode init_d3d12(window* w) {
	
	ID3D12Debug* d3dDebug = nullptr;

	HRESULT hr = D3D12GetDebugInterface(IID_PPV_ARGS(&d3dDebug));
	if (FAILED(hr)) {
		return ErrorCode::D3D12_INIT_FAILED;
	}

	d3dDebug->EnableDebugLayer();
	d3dDebug->Release();

	hr = D3D12CreateDevice(w->DXGIAdapter, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&w->D3D12Device));
	if (FAILED(hr)) {
		return ErrorCode::D3D12_INIT_FAILED;
	}
	SET_DEBUG_NAME(w->D3D12Device, "D3D12Device");
	

	D3D12_COMMAND_QUEUE_DESC desc;
	desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	desc.NodeMask = 0;

	hr = w->D3D12Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&w->D3D12GraphicsCommandQueue));
	if (FAILED(hr)) {
		return ErrorCode::D3D12_INIT_FAILED;
	}

	SET_DEBUG_NAME(w->D3D12GraphicsCommandQueue, "D3D12GraphicsCommandQueue");

	D3D12_DESCRIPTOR_HEAP_DESC rtvd;
	rtvd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvd.NumDescriptors = 16;
	rtvd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvd.NodeMask = 0;

	hr = w->D3D12Device->CreateDescriptorHeap(&rtvd, IID_PPV_ARGS(&w->D3D12RTVHeap));
	if (FAILED(hr)) {
		return ErrorCode::D3D12_INIT_FAILED;
	}

	SET_DEBUG_NAME(w->D3D12RTVHeap, "D3D12RtvHeap");

	D3D12_DESCRIPTOR_HEAP_DESC dsvd;
	dsvd.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsvd.NumDescriptors = 16;
	dsvd.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	dsvd.NodeMask = 0;

	hr = w->D3D12Device->CreateDescriptorHeap(&dsvd, IID_PPV_ARGS(&w->D3D12DSVHeap));
	if (FAILED(hr)) {
		return ErrorCode::D3D12_INIT_FAILED;
	}
	SET_DEBUG_NAME(w->D3D12DSVHeap, "D3D12DsvHeap");

	for (uint32_t i = 0; i < _countof(w->D3D12CommandAllocator); ++i) {
		hr = w->D3D12Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&w->D3D12CommandAllocator[i]));
		if (FAILED(hr)) {
			return ErrorCode::D3D12_INIT_FAILED;
		}

		hr = w->D3D12Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, w->D3D12CommandAllocator[i], nullptr, IID_PPV_ARGS(&w->D3D12CommandList[i]));
		if (FAILED(hr)) {
			return ErrorCode::D3D12_INIT_FAILED;
		}
	}

	return ErrorCode::SUCCESS;
}

ErrorCode init_swap_chain(window* w) {
	DXGI_SWAP_CHAIN_DESC1 desc1;

	RECT rect;
	GetClientRect(w->Window, &rect);

	desc1.Width              = (UINT)(rect.right - rect.left);
	desc1.Height             = (UINT)(rect.bottom - rect.top);
	desc1.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
	desc1.Stereo             = FALSE;
	desc1.SampleDesc.Count   = 1;
	desc1.SampleDesc.Quality = 0;
	desc1.BufferUsage        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	desc1.BufferCount        = _countof(w->D3D12CommandAllocator);
	desc1.Scaling            = DXGI_SCALING_NONE;
	desc1.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	desc1.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
	desc1.Flags              = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

	IDXGISwapChain1* swapChain = nullptr;
	HRESULT hr = w->DXGIFactory->CreateSwapChainForHwnd(
		w->D3D12GraphicsCommandQueue,
		w->Window,
		&desc1,
		nullptr,
		nullptr,
		&swapChain);

	if (FAILED(hr)) {
		return ErrorCode::DXGI_ERROR;
	}

	swapChain->QueryInterface(&w->DXGISwapChain);
	swapChain->Release();

	return ErrorCode::SUCCESS;
}


static void deinit_swap_chain(window* w) {
	SafeRelease(w->DXGISwapChain);
}

static void deinit_d3d12(window* w) {
	for (uint32_t i = 0; i < _countof(w->D3D12CommandAllocator); ++i) {
		SafeRelease(w->D3D12CommandAllocator[i]);
		SafeRelease(w->D3D12CommandList[i]);
	}

	SafeRelease(w->D3D12DSVHeap);
	SafeRelease(w->D3D12RTVHeap);

	SafeRelease(w->D3D12GraphicsCommandQueue);
	SafeRelease(w->D3D12Device);
}

static void deinit_dxgi(window* w) {

	SafeRelease(w->DXGIAdapter);
	SafeRelease(w->DXGIFactory);

	IDXGIDebug1* dxgiDebug;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebug))))
	{
		dxgiDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		SafeRelease(dxgiDebug);
	}
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: 
	{
		auto create_data = reinterpret_cast<CREATESTRUCT*>(lParam);
		auto wnd         = static_cast<window*>(create_data->lpCreateParams);
		wnd->Window      = hwnd;

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));

		if (init_dxgi(wnd) == ErrorCode::SUCCESS) {
			if (init_d3d12(wnd) == ErrorCode::SUCCESS) {
				if (init_swap_chain(wnd) == ErrorCode::SUCCESS) {
					
					return 0;
				}
			}
		}

		deinit_swap_chain(wnd);
		deinit_d3d12(wnd);
		deinit_dxgi(wnd);

		return -1;
	}

	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
	{
		auto wnd = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		deinit_swap_chain(wnd);
		deinit_d3d12(wnd);
		deinit_dxgi(wnd);

		clear_window(wnd);

		PostQuitMessage(0);

		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

ErrorCode create_window(HINSTANCE hInstance, window* wndOut)
{
	ZeroMemory(wndOut, sizeof(*wndOut));

	static LPCTSTR ClassName = TEXT("particulator");
	static ATOM ClassId      = 0;

	if (wndOut == nullptr) {
		return ErrorCode::INVALID_ARGUMENT;
	}

	if (hInstance == nullptr) {
		return ErrorCode::INVALID_ARGUMENT;
	}

	if (ClassId == 0) {
		WNDCLASS wc = {};
		wc.cbClsExtra = 0;
		wc.cbWndExtra = 0;
		wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
		wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
		wc.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
		wc.hInstance = hInstance;
		wc.lpfnWndProc = &WindowProc;
		wc.lpszClassName = ClassName;
		wc.lpszMenuName = nullptr;
		wc.style = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

		ClassId = RegisterClass(&wc);
		if (ClassId == 0) {
			return ErrorCode::REGISTER_CLASS_FAILED;
		}
	}

	const HWND hWnd = CreateWindow(
		ClassName,
		TEXT("MainWindow"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		800, 600, nullptr, nullptr, hInstance, wndOut);

	if (hWnd == nullptr) {
		return ErrorCode::CREATE_WINDOW_FAILED;
	}

	UpdateWindow(hWnd);

	wndOut->ClassId = ClassId;

	assert(wndOut->Window != nullptr);

	return ErrorCode::SUCCESS;
}

void show_window(window* wnd) {
	if (wnd != nullptr) {
		ShowWindow(wnd->Window, SW_SHOW);
	}
}
