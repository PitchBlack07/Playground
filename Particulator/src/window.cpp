#include "window.h"

static void clear_window(window* w) {
	w->ClassId = 0;
	w->Window  = nullptr;
}

static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_CREATE: 
	{
		auto create_data = reinterpret_cast<CREATESTRUCT*>(lParam);
		auto wnd         = static_cast<window*>(create_data->lpCreateParams);

		SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(wnd));
		return 0;
	}
	case WM_CLOSE:
		DestroyWindow(hwnd);
		return 0;

	case WM_DESTROY:
	{
		auto wnd = reinterpret_cast<window*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));

		clear_window(wnd);

		PostQuitMessage(0);

		return 0;
	}
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}

ErrorCode create_window(HINSTANCE hInstance, window* wndOut)
{
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

	HWND hWnd = CreateWindow(
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
	wndOut->Window  = hWnd;

	return ErrorCode::SUCCESS;
}

void destroy_window(window* wnd) {
	if (wnd != nullptr) {
		delete wnd;
	}
}

void show_window(window* wnd) {
	if (wnd != nullptr) {
		ShowWindow(wnd->Window, SW_SHOW);
	}ErrorCode create_window(HINSTANCE, window* wndOut);
}