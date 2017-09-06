#pragma once
#ifndef GPU_FB_H_INCLUDED
#define GPU_FB_H_INCLUDED

#include <d3d12.h>

struct gpu_fb
{
	UINT8 colormask;

	struct
	{
		ID3D12Resource* color[8];
		ID3D12Resource* depthstencil;
	} resources;

	struct
	{
		D3D12_CPU_DESCRIPTOR_HANDLE srv[8];
		D3D12_CPU_DESCRIPTOR_HANDLE rtv[8];
		D3D12_CPU_DESCRIPTOR_HANDLE dsv;
	} handles;

	struct
	{
		D3D12_RESOURCE_STATES color[8];
		D3D12_RESOURCE_STATES depthstencil;
	} states;
};

#endif
