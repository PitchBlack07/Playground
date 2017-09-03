#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "gpu.h"

#ifdef __cplusplus
extern "C" {
#endif

//!
//! \brief Creates a display
//!
bool display_create(void* hInstance_, uint32_t width_, uint32_t height_);

//!
//! \brief Destroys a display
//!
void display_destroy();

#ifdef __cplusplus
}
#endif
