#pragma once

#include <Windows.h>
#include "error.h"

struct window
{
	ATOM ClassId;
	HWND Window;
};


ErrorCode create_window(HINSTANCE, window* wndOut);

void show_window(window* wnd);
void destroy_window(window* wnd);