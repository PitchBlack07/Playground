#include "Display.h"
#include <malloc.h>
#include <Windows.h>
#include <assert.h>

static const TCHAR* ClassName = TEXT("display_window_class");

static HINSTANCE hInstance                   = nullptr;
static HWND hwnd                             = nullptr;

static LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch (msg_)
	{
	case WM_CREATE:
		return gpu_init(hwnd_) ? 0 : -1;

	case WM_CLOSE:
		gpu_deinit();
		DestroyWindow(hwnd_);

		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
	}
}

static bool register_window_class(HINSTANCE hInstance_)
{
}

static bool create_window(HINSTANCE hInstance_, uint32_t width_, uint32_t height_)
{
	hInstance = hInstance_;

	if (register_window_class(hInstance_))
	{

		return hwnd != nullptr;
	}
	return false;
}

bool display_create(void* hInstance_, uint32_t width_, uint32_t height_)
{
	if (!hInstance) {
		hInstance = (HINSTANCE)hInstance_;
	}
	
	WNDCLASS wc;

	if (!GetClassInfo(hInstance, ClassName, &wc)) {

		wc.cbClsExtra    = 0;
		wc.cbWndExtra    = 0;
		wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_BACKGROUND);
		wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
		wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
		wc.hInstance     = hInstance;
		wc.lpfnWndProc   = &WndProc;
		wc.lpszClassName = ClassName;
		wc.lpszMenuName  = nullptr;
		wc.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;

		if (!RegisterClass(&wc)) {
			return false;
		}
	}

	hwnd = CreateWindow(ClassName, TEXT("Display"), WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, (INT)width_, (INT)height_, nullptr, nullptr, hInstance, nullptr);

	if (!hwnd) {
		return false;
	}

	ShowWindow(hwnd, SW_SHOW);

	return true;
}

void display_destroy()
{
	hwnd = NULL;
	UnregisterClass(ClassName, hInstance);
}
