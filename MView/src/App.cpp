#include <MView/MView.h>
#include <MView/App.h>
#include <MView/Utils.h>

#include <assert.h>
#include <dxgi1_4.h>
#include <d3d12.h>

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

// DXGI SwapChain
IDXGISwapChain3* dxgiSwapChain = nullptr;

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

    return TRUE;
}

static BOOL CreateSwapChainObject(HWND window_)
{
    RECT rect;
    DXGI_SWAP_CHAIN_DESC1           scd;
    DXGI_SWAP_CHAIN_FULLSCREEN_DESC scfd;

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
    scd.Flags              = 0;
    
    UINT modeCount;
    dxgiOutput->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modeCount, NULL);
    
    DXGI_MODE_DESC1* const md = static_cast<DXGI_MODE_DESC1*>(malloc(sizeof(DXGI_MODE_DESC1) * modeCount));
    if (md) {
        dxgiOutput->GetDisplayModeList1(DXGI_FORMAT_R8G8B8A8_UNORM, 0, &modeCount, md);

        scfd.RefreshRate      = md[modeCount - 1].RefreshRate;
        scfd.Scaling          = md[modeCount - 1].Scaling;
        scfd.ScanlineOrdering = md[modeCount - 1].ScanlineOrdering;
        scfd.Windowed         = TRUE;

        free(md);

        if (FAILED(dxgiFactory->CreateSwapChainForHwnd(d3dGraphicsQ, window_, &scd, &scfd, dxgiOutput, &sc))) {
            return FALSE;
        }

        if (FAILED(sc->QueryInterface(__uuidof(*dxgiSwapChain), reinterpret_cast<void**>(&dxgiSwapChain)))) {
            SafeRelease(sc);
            return FALSE;
        }
    }


    SafeRelease(sc);
    return TRUE;
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

    return 0;
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

    SafeRelease(d3dGraphicsQ);
    SafeRelease(d3dDevice);
}

static void DestroySwapChainObject()
{
    SafeRelease(dxgiSwapChain);
}

static LRESULT OnDestroy(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    DestroySwapChainObject();
    DestroyD3D12Objects();
    DestroyDXGIObjects();

    PostQuitMessage(0);
    return 0;
}

static LRESULT CALLBACK MViewWndProc(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    switch(msg_)
    {
    case WM_CREATE:  return OnCreate(wnd_, msg_, wparam_, lparam_);
    case WM_CLOSE:   return OnClose(wnd_, msg_, wparam_, lparam_);
    case WM_DESTROY: return OnDestroy(wnd_, msg_, wparam_, lparam_);
    default:         return DefWindowProc(wnd_, msg_, wparam_, lparam_);
    }
}