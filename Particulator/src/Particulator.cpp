#include <Windows.h>
#include "window.h"


int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow)
{
	window wnd;
	auto error = create_window(hInstance, &wnd);
	
	show_window(&wnd);

	MSG msg = {};

	while (GetMessage(&msg, nullptr, 0, 0)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}