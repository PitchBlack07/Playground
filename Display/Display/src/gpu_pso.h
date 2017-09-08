#pragma once
#ifndef GPU_BLEND_STATE_H_INCLUDED
#define GPU_BLEND_STATE_H_INCLUDED

#include <d3d12.h>

D3D12_BLEND_DESC gpu_create_default_blend_desc();
D3D12_RASTERIZER_DESC gpu_create_default_raster_desc();
D3D12_DEPTH_STENCIL_DESC gpu_create_default_depth_stencil_desc();

void gpu_gps_init_default(D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc_);

ID3D12PipelineState* gpu_create_pso(ID3D12RootSignature* signature_, ID3D12Device* d3dDevice_);

#endif
