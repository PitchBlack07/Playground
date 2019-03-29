
#include <Windows.h>
#include <vulkan.h>

static ATOM       WndClass;
static HWND       hWnd;
static HINSTANCE  hInstance;
static VkInstance vkInstance;

static LRESULT CALLBACK vktWndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch (msg_)
	{
	case WM_CLOSE:
		DestroyWindow(hwnd_);
		return 0;

	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	default:
		return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
	}
}

int32_t vktCreateWindow(uint32_t width_, uint32_t height_)
{
	hInstance = GetModuleHandle(NULL);
	WNDCLASS wc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon         = LoadIcon(NULL, IDI_APPLICATION);
	wc.hInstance     = hInstance;
	wc.lpfnWndProc   = vktWndProc;
	wc.lpszClassName = TEXT("vktApplicationWindow");
	wc.lpszMenuName  = NULL;
	wc.style         = CS_VREDRAW | CS_HREDRAW | CS_OWNDC;

	WndClass = RegisterClass(&wc);
	if (WndClass == 0) {
		return 0;
	}

	hWnd = CreateWindow((LPCTSTR)WndClass, TEXT("vtkWindow"), WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, CW_USEDEFAULT, width_, height_, NULL, NULL, hInstance, NULL);

	if (hWnd == NULL) {
		return 0;
	}

	return 1;
}

int32_t vktStartMessageLoop() {

	ShowWindow(hWnd, SW_SHOW);
	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}
	return (int32_t) msg.lParam;
}