#pragma once

#include <d3d12.h>

BOOL gpu_text_drawer_create();
void gpu_text_drawer_destroy();

void gpu_draw_text(ID3D12GraphicsCommandList* cmdlist_, INT32 x_, INT32 y_, LPCTSTR text_);

