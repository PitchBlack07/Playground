#include <Windows.h>
#include "Display.h"
#include "gpu.h"

static INT app_run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(msg));

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (INT)msg.wParam;
}

INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPSTR lpCmdLine, INT iCmdShow)
{
	INT retval = 0;

	display_create(hInstance, 800, 480);

	retval = app_run();

	display_destroy();

	return retval;
}