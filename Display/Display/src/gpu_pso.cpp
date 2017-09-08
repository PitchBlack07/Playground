#pragma once
#include "gpu_pso.h"

D3D12_BLEND_DESC gpu_create_default_blend_desc()
{
	D3D12_BLEND_DESC bd = {};

	bd.AlphaToCoverageEnable  = FALSE;
	bd.IndependentBlendEnable = FALSE;

	for (UINT i = 0; i < _countof(bd.RenderTarget); ++i) {

		bd.RenderTarget[i].BlendEnable           = FALSE;
		bd.RenderTarget[i].BlendOp               = D3D12_BLEND_OP_ADD;
		bd.RenderTarget[i].BlendOpAlpha          = D3D12_BLEND_OP_ADD;
		bd.RenderTarget[i].DestBlend             = D3D12_BLEND_ZERO;
		bd.RenderTarget[i].DestBlendAlpha        = D3D12_BLEND_ZERO;
		bd.RenderTarget[i].LogicOp               = D3D12_LOGIC_OP_CLEAR;
		bd.RenderTarget[i].LogicOpEnable         = FALSE;
		bd.RenderTarget[i].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
		bd.RenderTarget[i].SrcBlend              = D3D12_BLEND_ONE;
		bd.RenderTarget[i].SrcBlendAlpha         = D3D12_BLEND_ONE;
	}

	return bd;
}

D3D12_RASTERIZER_DESC gpu_create_default_raster_desc()
{
	D3D12_RASTERIZER_DESC rd;
	rd.AntialiasedLineEnable = FALSE;
	rd.ConservativeRaster    = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
	rd.CullMode              = D3D12_CULL_MODE_NONE;
	rd.DepthBias             = 0;
	rd.DepthBiasClamp        = 0.f;
	rd.DepthClipEnable       = TRUE;
	rd.FillMode              = D3D12_FILL_MODE_SOLID;
	rd.ForcedSampleCount     = 0;
	rd.FrontCounterClockwise = FALSE;
	rd.MultisampleEnable     = FALSE;
	rd.SlopeScaledDepthBias  = 0.f;

	return rd;
}

D3D12_DEPTH_STENCIL_DESC gpu_create_default_depth_stencil_desc()
{
	D3D12_DEPTH_STENCIL_DESC dsd;
	dsd.DepthEnable                  = TRUE;
	dsd.DepthWriteMask               = D3D12_DEPTH_WRITE_MASK_ALL;
	dsd.DepthFunc                    = D3D12_COMPARISON_FUNC_LESS;
	dsd.StencilEnable                = FALSE;
	dsd.StencilReadMask              = 0xff;
	dsd.StencilWriteMask             = 0xff;
	dsd.FrontFace.StencilDepthFailOp = D3D12_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFailOp      = D3D12_STENCIL_OP_KEEP;
	dsd.FrontFace.StencilFunc        = D3D12_COMPARISON_FUNC_ALWAYS;
	dsd.FrontFace.StencilPassOp      = D3D12_STENCIL_OP_KEEP;
	dsd.BackFace                     = dsd.FrontFace;

	return dsd;
}

void gpu_gps_init_default(D3D12_GRAPHICS_PIPELINE_STATE_DESC* desc_)
{
	memset(desc_, 0, sizeof(*desc_));

	desc_->pRootSignature                  = nullptr;
	desc_->BlendState                      = gpu_create_default_blend_desc();
	desc_->SampleMask                      = 0xffffffff;
	desc_->RasterizerState                 = gpu_create_default_raster_desc();
	desc_->DepthStencilState               = gpu_create_default_depth_stencil_desc();
	desc_->InputLayout.NumElements         = 0;
	desc_->InputLayout.pInputElementDescs  = NULL;
	desc_->IBStripCutValue                 = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
	desc_->PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	desc_->DSVFormat                       = DXGI_FORMAT_UNKNOWN;
	desc_->SampleDesc.Count                = 1;
	desc_->SampleDesc.Quality              = 0;
	desc_->CachedPSO.CachedBlobSizeInBytes = 0;
	desc_->CachedPSO.pCachedBlob           = NULL;
	desc_->NodeMask                        = 0x00;
	desc_->Flags                           = D3D12_PIPELINE_STATE_FLAG_NONE;

	for (UINT i = 0; i < _countof(desc_->RTVFormats); ++i) 
		desc_->RTVFormats[i] = DXGI_FORMAT_UNKNOWN;
}

ID3D12PipelineState* gpu_create_pso(ID3D12RootSignature* signature_, ID3D12Device* d3dDevice_)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psd = {};

	psd.pRootSignature                  = signature_;
	psd.BlendState                      = gpu_create_default_blend_desc();
	psd.SampleMask                      = 0xffffffff;
	psd.RasterizerState                 = gpu_create_default_raster_desc();
	psd.DepthStencilState               = gpu_create_default_depth_stencil_desc();
	psd.InputLayout.NumElements         = 0;
	psd.InputLayout.pInputElementDescs  = NULL;
	psd.IBStripCutValue                 = D3D12_INDEX_BUFFER_STRIP_CUT_VALUE_0xFFFFFFFF;
	psd.PrimitiveTopologyType           = D3D12_PRIMITIVE_TOPOLOGY_TYPE_UNDEFINED;
	//psd.RTVFormats
	psd.DSVFormat                       = DXGI_FORMAT_UNKNOWN;
	psd.SampleDesc.Count                = 1;
	psd.SampleDesc.Quality              = 0;
	psd.CachedPSO.CachedBlobSizeInBytes = 0;
	psd.CachedPSO.pCachedBlob           = NULL;
	psd.NodeMask                        = 0x00;
	psd.Flags                           = D3D12_PIPELINE_STATE_FLAG_NONE;

	ID3D12PipelineState* pso = nullptr;
	d3dDevice_->CreateGraphicsPipelineState(&psd, __uuidof(*pso), (void**)&pso);

	return pso;

	return NULL;
}