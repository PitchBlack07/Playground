#include <MView\MView.h>
#include <MView\App.h>

// Constants
static const wchar_t* ClassName = L"MViewWndClass";

// Window/Application parameters
static HINSTANCE Instance = nullptr;
static HWND Window        = nullptr;

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

static LRESULT OnCreate(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    return 0;
}

static LRESULT OnClose(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
    DestroyWindow(wnd_);
    return 0;
}

static LRESULT OnDestroy(HWND wnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
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