#include <MView/MView.h>
#include <MView/App.h>
#include <MView/Utils.h>

#include <assert.h>
#include <dxgi1_4.h>
#include <d3d12.h>
#include <d3d11on12.h>
#include <d2d1.h>
#include <d2d1_3.h>

// Constants
static const wchar_t* ClassName        = L"MViewWndClass";
static const UINT SwapChainBufferCount = 3;

// Window/Application parameters
static HINSTANCE Instance = nullptr;
static HWND Window        = nullptr;

// DXGI objects
IDXGIFactory2* dxgiFactory = nullptr;
IDXGIAdapter3* dxgiAdapter = nullptr;
IDXGIOutput4*  dxgiOutput  = nullptr;

// D3D12 objects
D3D_FEATURE_LEVEL d3dFeatureLevel;

ID3D12Device* d3dDevice           = nullptr;
ID3D12CommandQueue* d3dGraphicsQ  = nullptr;
ID3D12DescriptorHeap* d3dRtvHeap  = nullptr;
ID3D12Fence* d3dFence             = nullptr;
HANDLE d3dEvent                   = nullptr;
UINT64 d3dFenceId                 = 0;

ID3D12CommandAllocator* d3dAllocator[SwapChainBufferCount];
ID3D12GraphicsCommandList* d3dGraphicsCommandList[SwapChainBufferCount];
D3D12_CPU_DESCRIPTOR_HANDLE d3dRTV[SwapChainBufferCount];
ID3D12Resource* d3dRTVResources[SwapChainBufferCount];

// DXGI SwapChain
IDXGISwapChain3* dxgiSwapChain = nullptr;
HANDLE dxgiWaitableObject      = nullptr;

// D3D11on12
ID3D11Device*        d3d11Device;
ID3D11DeviceContext* d3d11Context;
ID3D11On12Device*    d3d11On12Device;
ID3D11Texture2D*     d3d11BackBuffer[SwapChainBufferCount];

ID2D1Factory3*      d2dFactory;
ID2D1Device*        d2dDevice;
ID2D1DeviceContext* d2dContext;
ID2D1Bitmap1*       d2dRenderTarget[SwapChainBufferCount];

// Window procedure
static LRESULT CALLBACK MViewWndProc(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_);

bool MViewInit(HINSTANCE hInstance_)
{
    WNDCLASS wc;
    wc.style         = CS_VREDRAW | CS_HREDRAW;
    wc.lpfnWndProc   = &MViewWndProc;
    wc.cbClsExtra    = 0;
    wc.cbWndExtra    = 0;
    wc.hInstance     = hInstance_;
    wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
    wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
    wc.lpszMenuName  = nullptr;
    wc.lpszClassName = ClassName;

    if (RegisterClass(&wc)) {
        Instance = hInstance_;
        return true;
    }
    else {
        return false;
    }
}

void MViewDeinit()
{
    if (Instance) {
        UnregisterClass(ClassName, Instance);
        Instance = nullptr;
    }
}

INT MViewRun(UINT width_, UINT height_)
{
    Window = CreateWindow(ClassName, L"MView", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, width_, height_, nullptr, nullptr, Instance, nullptr);
    if (Window == nullptr) {
        return -1;
    }

    ShowWindow(Window, SW_SHOW);

    MSG msg = {};
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (INT)msg.wParam;
}

static void WaitForGPU()
{
    d3dFenceId++;
    d3dGraphicsQ->Signal(d3dFence, d3dFenceId);
    d3dFence->SetEventOnCompletion(d3dFenceId, d3dEvent);

    WaitForSingleObject(d3dEvent, INFINITE);
}

static BOOL CreateDXGIObjects()
{
    IDXGIAdapter1* adapter1 = nullptr;
    IDXGIOutput* output     = nullptr;
    UINT dxgiFactoryFlags   = 0;

#ifdef _DEBUG
    dxgiFactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

    if (FAILED(CreateDXGIFactory2(dxgiFactoryFlags, __uuidof(*dxgiFactory), reinterpret_cast<void**>(&dxgiFactory)))) {
        goto create_dxgi_objects_failed;
    }

    
    if (FAILED(dxgiFactory->EnumAdapters1(0, &adapter1))) {
        goto create_dxgi_objects_failed;
    }

    if (FAILED(adapter1->QueryInterface(__uuidof(*dxgiAdapter), reinterpret_cast<void**>(&dxgiAdapter)))) {
        goto create_dxgi_objects_failed;
    }
    
    if (FAILED(dxgiAdapter->EnumOutputs(0, &output))) {
        goto create_dxgi_objects_failed;
    }

    if (FAILED(output->QueryInterface(__uuidof(*dxgiOutput), reinterpret_cast<void**>(&dxgiOutput)))) {
        goto create_dxgi_objects_failed;
    }

    SafeRelease(output);
    SafeRelease(adapter1);

    return true;

create_dxgi_objects_failed:
    SafeRelease(output);
    SafeRelease(adapter1);

    return false;
}

static BOOL CreateD3D12Objects()
{
    assert(dxgiAdapter);

#ifdef _DEBUG
    ID3D12Debug* d3dDebug;
    if (SUCCEEDED(D3D12GetDebugInterface(__uuidof(*d3dDebug), reinterpret_cast<void**>(&d3dDebug))))
    {
        d3dDebug->EnableDebugLayer();
    }
    SafeRelease(d3dDebug);
#endif

    const D3D_FEATURE_LEVEL levels[] =
    {
        D3D_FEATURE_LEVEL_12_1,
        D3D_FEATURE_LEVEL_12_0,
        D3D_FEATURE_LEVEL_11_1,
        D3D_FEATURE_LEVEL_11_0
    };

    for (UINT i = 0; i < _countof(levels); ++i)
    {
        const HRESULT hr = D3D12CreateDevice(dxgiAdapter, levels[i], __uuidof(*d3dDevice), reinterpret_cast<void**>(&d3dDevice));
        if (SUCCEEDED(hr)) {
            d3dFeatureLevel = levels[i];
            break;
        }
    }

    if (!d3dDevice) {
        return FALSE;
    }

    D3D12_COMMAND_QUEUE_DESC cqd;
    cqd.Type     = D3D12_COMMAND_LIST_TYPE_DIRECT;
    cqd.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
    cqd.Flags    = D3D12_COMMAND_QUEUE_FLAG_NONE;
    cqd.NodeMask = 0x00;

    if (FAILED(d3dDevice->CreateCommandQueue(&cqd, __uuidof(*d3dGraphicsQ), reinterpret_cast<void**>(&d3dGraphicsQ))))
    {
        return FALSE;
    }

    D3D12_DESCRIPTOR_HEAP_DESC rtvd;
    rtvd.Type           = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
    rtvd.NumDescriptors = SwapChainBufferCount;
    rtvd.Flags          = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
    rtvd.NodeMask       = 0x00;

    if (FAILED(d3dDevice->CreateDescriptorHeap(&rtvd, __uuidof(*d3dRtvHeap), reinterpret_cast<void**>(&d3dRtvHeap))))
    {
        return FALSE;
    }

    for (UINT i = 0; i < SwapChainBufferCount; ++i)
    {
        if (FAILED(d3dDevice->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(*d3dAllocator[i]), reinterpret_cast<void**>(&d3dAllocator[i])))) {
            return FALSE;
        }

        if (FAILED(d3dDevice->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dAllocator[i], nullptr, __uuidof(*d3dGraphicsCommandList[i]), reinterpret_cast<void**>(&d3dGraphicsCommandList[i]))))
        {
            return FALSE;
        }

        d3dGraphicsCommandList[i]->Close();
    }

    if (FAILED(d3dDevice->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(*d3dFence), reinterpret_cast<void**>(&d3dFence))))
    {
        return FALSE;
    }

    d3dEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("SYNCEVENT"));
    if (!d3dEvent)
    {
        return FALSE;
    }

    return TRUE;
}

static BOOL CreateSwapChainObject(HWND window_)
{
    RECT rect;
    DXGI_SWAP_CHAIN_DESC1           scd;

    IDXGISwapChain1* sc = nullptr;

    GetClientRect(window_, &rect);

    scd.Width              = rect.right - rect.left;
    scd.Height             = rect.bottom - rect.top;
    scd.Format             = DXGI_FORMAT_R8G8B8A8_UNORM;
    scd.Stereo             = FALSE;
    scd.SampleDesc.Count   = 1;
    scd.SampleDesc.Quality = 0;
    scd.BufferUsage        = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
    scd.BufferCount        = SwapChainBufferCount;
    scd.Scaling            = DXGI_SCALING_NONE;
    scd.SwapEffect         = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
    scd.AlphaMode          = DXGI_ALPHA_MODE_UNSPECIFIED;
    scd.Flags              = DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;

    if (FAILED(dxgiFactory->CreateSwapChainForHwnd(d3dGraphicsQ, window_, &scd, nullptr, nullptr, &sc))) {
        return FALSE;
    }

    if (FAILED(sc->QueryInterface(__uuidof(*dxgiSwapChain), reinterpret_cast<void**>(&dxgiSwapChain)))) {
        dxgiSwapChain->SetMaximumFrameLatency(SwapChainBufferCount);
        dxgiWaitableObject = dxgiSwapChain->GetFrameLatencyWaitableObject();
        SafeRelease(sc);
        return FALSE;
    }

    SafeRelease(sc);
    return TRUE;
}

static BOOL CreateSwapChainRenderTargetViews()
{
    assert(d3dRtvHeap);

    D3D12_CPU_DESCRIPTOR_HANDLE rtvhandle = d3dRtvHeap->GetCPUDescriptorHandleForHeapStart();
    const UINT handleSize                 = d3dDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

    for (UINT i = 0; i < SwapChainBufferCount; ++i)
    {
        HRESULT hr = dxgiSwapChain->GetBuffer(i, __uuidof(*d3dRTVResources[i]), reinterpret_cast<void**>(&d3dRTVResources[i]));
        if (FAILED(hr)) {
            return FALSE;
        }

        D3D12_RENDER_TARGET_VIEW_DESC rtvd;
        rtvd.ViewDimension        = D3D12_RTV_DIMENSION_TEXTURE2D;
        rtvd.Format               = DXGI_FORMAT_R8G8B8A8_UNORM;
        rtvd.Texture2D.MipSlice   = 0;
        rtvd.Texture2D.PlaneSlice = 0;
        
        d3dRTV[i].ptr = rtvhandle.ptr + i * handleSize;

        d3dDevice->CreateRenderTargetView(d3dRTVResources[i], &rtvd, d3dRTV[i]);
    }

    return TRUE;
}

static void DestroySwapChainRenderTargetViews()
{
    memset(d3dRTV, 0, sizeof(d3dRTV));
    for (UINT i = 0; i < SwapChainBufferCount; ++i)
    {
        SafeRelease(d3dRTVResources[i]);
    }
}

static BOOL CreateD3D11Objects()
{
    IUnknown* q[] = { d3dGraphicsQ };

    HRESULT hr = D3D11On12CreateDevice(d3dDevice, D3D11_CREATE_DEVICE_SINGLETHREADED | D3D11_CREATE_DEVICE_BGRA_SUPPORT, nullptr, 0, q, 1, 0, &d3d11Device, &d3d11Context, nullptr);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = d3d11Device->QueryInterface(__uuidof(ID3D11On12Device), (void**)&d3d11On12Device);
    if (FAILED(hr)) {
        return FALSE;
    }


    return TRUE;
}

static BOOL CreateD3D11BackBufferTextures()
{
    for (UINT i = 0; i < _countof(d3d11BackBuffer); ++i)
    {
        D3D11_RESOURCE_FLAGS flags;
        flags.BindFlags           = D3D11_BIND_RENDER_TARGET;
        flags.MiscFlags           = 0;
        flags.CPUAccessFlags      = 0;
        flags.StructureByteStride = 0;

        HRESULT hr = d3d11On12Device->CreateWrappedResource(d3dRTVResources[i], &flags, D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT, __uuidof(ID3D11Texture2D), (void**)&d3d11BackBuffer[i]);
        if (FAILED(hr)) {
            return FALSE;
        }
    }

    return TRUE;
}

void DestroyD3D11Objects()
{
    SafeRelease(d3d11On12Device);
    SafeRelease(d3d11Context);
    SafeRelease(d3d11Device);
}

void DestroyD3D11BackBufferTextures()
{
    for (UINT i = 0; i < _countof(d3d11BackBuffer); ++i) {
        SafeRelease(d3d11BackBuffer[i]);
    }
}

static BOOL CreateD2D1Objects() 
{
    IDXGIDevice* dxgiDevice = nullptr;

    D2D1_FACTORY_OPTIONS options = { D2D1_DEBUG_LEVEL_INFORMATION };

    HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(*d2dFactory), &options, (void**)&d2dFactory);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = d3d11On12Device->QueryInterface(__uuidof(*dxgiDevice), (void**)&dxgiDevice);
    if (FAILED(hr)) {
        return FALSE;
    }

    hr = d2dFactory->CreateDevice(dxgiDevice, &d2dDevice);
    if (FAILED(hr)) {
        SafeRelease(dxgiDevice);
        return FALSE;
    }

    SafeRelease(dxgiDevice);

    hr = d2dDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &d2dContext);
    if (FAILED(hr)) {
        return hr;
    }

    return TRUE;
}

static void DestroyD2D1Objects()
{
    SafeRelease(d2dDevice);
    SafeRelease(d2dContext);
    SafeRelease(d2dFactory);
}

static BOOL CreateD2D1Bitmaps()
{
    float dpix, dpiy;
    d2dFactory->GetDesktopDpi(&dpix, &dpiy);

    D2D1_BITMAP_PROPERTIES1 bitmapProperties;
    bitmapProperties.pixelFormat.format    = DXGI_FORMAT_UNKNOWN;
    bitmapProperties.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
    bitmapProperties.dpiX                  = dpix;
    bitmapProperties.dpiY                  = dpiy;
    bitmapProperties.bitmapOptions         = D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW;
    bitmapProperties.colorContext          = nullptr;

    for (UINT i = 0; i < _countof(d2dRenderTarget); ++i) {
        IDXGISurface* surface = nullptr;
        
        HRESULT hr = d3d11BackBuffer[i]->QueryInterface(__uuidof(*surface), (void**)&surface);
        if (FAILED(hr)) {
            return FALSE;
        }

        hr = d2dContext->CreateBitmapFromDxgiSurface(surface, &bitmapProperties, &d2dRenderTarget[i]);
        SafeRelease(surface);
        if (FAILED(hr)) {
            return FALSE;
        }
    }
    return TRUE;
}

static void DestroyD2D1Bitmaps()
{
    for (UINT i = 0; i < _countof(d2dRenderTarget); ++i) {
        SafeRelease(d2dRenderTarget[i]);
    }
}

static LRESULT OnCreate(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    if (!CreateDXGIObjects()) {
        return -1;
    }

    if (!CreateD3D12Objects()) {
        return -1;
    }

    if (!CreateSwapChainObject(wnd_)) {
        return -1;
    }

    if (!CreateSwapChainRenderTargetViews()) {
        return -1;
    }

    if (!CreateD3D11Objects()) {
        return -1;
    }

    if (!CreateD3D11BackBufferTextures()) {
        return -1;
    }

    if (!CreateD2D1Objects()) {
        return -1;
    }

    if (!CreateD2D1Bitmaps()) {
        return -1;
    }

    return 0;
}

static LRESULT OnEnterSizeMove(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    return 0;
}

static LRESULT OnSize(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    INT width  = (INT)LOWORD(lparam_);
    INT height = (INT)HIWORD(lparam_);

    if (dxgiSwapChain)
    {
        WaitForGPU();

        DestroyD2D1Bitmaps();
        DestroyD3D11BackBufferTextures();
        DestroySwapChainRenderTargetViews();
        d3d11Context->Flush();
        WaitForGPU();

        DXGI_SWAP_CHAIN_DESC1 desc;
        dxgiSwapChain->GetDesc1(&desc);
        if (FAILED(dxgiSwapChain->ResizeBuffers(SwapChainBufferCount, width, height, desc.Format, desc.Flags)))
        {
            return -1;
        }
        CreateSwapChainRenderTargetViews();
        CreateD3D11BackBufferTextures();
        CreateD2D1Bitmaps();
    }
    return 0;
}

static LRESULT OnExitSizeMove(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparma_)
{
    return 0;
}

static LRESULT OnPaint(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    FLOAT clear[] = { 0, 0, 1, 1 };

    if (dxgiSwapChain)
    {
        // wait for the swap chain to be ready
        WaitForSingleObject(dxgiWaitableObject, INFINITE);

        UINT idx = dxgiSwapChain->GetCurrentBackBufferIndex();
        
        d3dAllocator[idx]->Reset();
        d3dGraphicsCommandList[idx]->Reset(d3dAllocator[idx], nullptr);

        D3D12_RESOURCE_BARRIER barrier;
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = d3dRTVResources[idx];
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = 0;

        d3dGraphicsCommandList[idx]->ResourceBarrier(1, &barrier);

        d3dGraphicsCommandList[idx]->ClearRenderTargetView(d3dRTV[idx], clear, 0, nullptr);
        d3dGraphicsCommandList[idx]->Close();
        ID3D12CommandList* cmdlist[] = { d3dGraphicsCommandList[idx] };

        d3dGraphicsQ->ExecuteCommandLists(1, cmdlist);

        
        
        ID3D11Resource* rrr = { d3d11BackBuffer[idx] };
        d3d11On12Device->AcquireWrappedResources(&rrr, 1);

        d2dContext->BeginDraw();
        d2dContext->SetTarget(d2dRenderTarget[idx]);
        
        D2D1_RECT_F f;
        f.bottom = 100;
        f.left = 0;
        f.right = 100;
        f.top = 0;

        D2D1_COLOR_F color = { 0, 1, 0, 1 };
        ID2D1SolidColorBrush* brush = nullptr;
        d2dContext->CreateSolidColorBrush(color, &brush);
        
        
        d2dContext->FillRectangle(f, brush);
        
        SafeRelease(brush);
        d2dContext->EndDraw();

        d2dContext->BeginDraw();
        d2dContext->SetTarget(NULL);
        d2dContext->EndDraw();

        d3d11On12Device->ReleaseWrappedResources(&rrr, 1);
        d3d11Context->Flush();
        
        //barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        //barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        //barrier.Transition.pResource = d3dRTVResources[idx];
        //barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        //barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        //barrier.Transition.Subresource = 0;

        //d3dGraphicsCommandList[idx]->ResourceBarrier(1, &barrier);


        HRESULT hr = dxgiSwapChain->Present(0, 0);
        
        assert(SUCCEEDED(hr));
    }
    return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
}

static LRESULT OnClose(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    DestroyWindow(wnd_);
    return 0;
}

static void DestroyDXGIObjects()
{
    SafeRelease(dxgiOutput);
    SafeRelease(dxgiAdapter);
    SafeRelease(dxgiFactory);
}

static void DestroyD3D12Objects()
{
    d3dFeatureLevel = static_cast<D3D_FEATURE_LEVEL>(0);

    CloseHandle(d3dEvent);
    d3dEvent = nullptr;

    SafeRelease(d3dFence);

    for (UINT i = 0; i < SwapChainBufferCount; ++i)
    {
        SafeRelease(d3dGraphicsCommandList[i]);
        SafeRelease(d3dAllocator[i]);
    }

    SafeRelease(d3dRtvHeap);
    SafeRelease(d3dGraphicsQ);
    SafeRelease(d3dDevice);
}

static void DestroySwapChainObject()
{
    SafeRelease(dxgiSwapChain);
}

static LRESULT OnDestroy(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    WaitForGPU();

    DestroyD2D1Bitmaps();
    DestroyD2D1Objects();
    DestroyD3D11BackBufferTextures();
    DestroyD3D11Objects();
    DestroySwapChainRenderTargetViews();
    DestroySwapChainObject();
    DestroyD3D12Objects();
    DestroyDXGIObjects();

    PostQuitMessage(0);
    return 0;
}

#define WND_PROC_DISPATCH(msg, func) case msg: return func(wnd_, msg_, wparam_, lparam_)
#define WND_PROC_DISPATCH_DEFAULT default: return DefWindowProc(wnd_, msg_, wparam_, lparam_)

static LRESULT CALLBACK MViewWndProc(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    switch(msg_)
    {
    WND_PROC_DISPATCH(WM_CREATE       , OnCreate       );
    WND_PROC_DISPATCH(WM_ENTERSIZEMOVE, OnEnterSizeMove);
    WND_PROC_DISPATCH(WM_SIZE         , OnSize         );
    WND_PROC_DISPATCH(WM_EXITSIZEMOVE , OnExitSizeMove );
    WND_PROC_DISPATCH(WM_PAINT        , OnPaint        );
    WND_PROC_DISPATCH(WM_CLOSE        , OnClose        );
    WND_PROC_DISPATCH(WM_DESTROY      , OnDestroy      );

    WND_PROC_DISPATCH_DEFAULT;
    }
}