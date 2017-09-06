#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "gpu.h"

#ifdef __cplusplus
extern "C" {
#endif

	typedef LRESULT(CALLBACK *display_handler)(HWND, UINT, WPARAM, LPARAM);

//!
//! \brief Creates a display
//!
bool display_create(void* hInstance_, uint32_t width_, uint32_t height_);

//!
//! \brief Destroys a display
//!
void display_destroy();

void display_register_handler(UINT msg_, display_handler handler_);

#ifdef __cplusplus
}
#endif
