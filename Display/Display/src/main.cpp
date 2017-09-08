
#include <Windows.h>
#include "gpu.h"
#include "gpu_pso.h"
#include "utils.h"
#include <d3d12.h>
#include "shader\quad.h"
#include "shader\quad_ps.h"

ID3D12PipelineState* d3dPso;
ID3D12RootSignature* d3dSignature;

BOOL frame_init()
{
	D3D12_ROOT_SIGNATURE_DESC rsd;
	rsd.Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
	rsd.NumParameters = 0;
	rsd.NumStaticSamplers = 0;
	rsd.pParameters = nullptr;
	rsd.pStaticSamplers = nullptr;

	ID3DBlob* signature, *error;

	D3D12SerializeRootSignature(&rsd, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	gpudrv.d3dDevice->CreateRootSignature(0x00, signature->GetBufferPointer(), signature->GetBufferSize(), __uuidof(*d3dSignature), (void**)&d3dSignature);

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psd;
	gpu_gps_init_default(&psd);

	psd.VS.BytecodeLength  = sizeof(vs_quad_main);
	psd.VS.pShaderBytecode = vs_quad_main;
	psd.PS.BytecodeLength  = sizeof(ps_quad_main);
	psd.PS.pShaderBytecode = ps_quad_main;

	psd.RTVFormats[0]         = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	psd.DSVFormat             = DXGI_FORMAT_D24_UNORM_S8_UINT;
	psd.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psd.NumRenderTargets      = 1;
	psd.pRootSignature        = d3dSignature;

	BOOL ok = SUCCEEDED(gpudrv.d3dDevice->CreateGraphicsPipelineState(&psd, __uuidof(*d3dPso), (void**)&d3dPso));

	SAFE_RELEASE(signature);

	return ok;
}

void frame_deinit()
{
	SAFE_RELEASE(d3dSignature);
	SAFE_RELEASE(d3dPso);
}

void draw_frame()
{
	gpu_begin_frame();

	ID3D12CommandAllocator* d3dAllocator = gpu_get_command_allocator();

	d3dAllocator->Reset();

	ID3D12Device* device = gpu_get_device();
	ID3D12GraphicsCommandList* d3dCmdList;

	if (FAILED(device->CreateCommandList(0x00, D3D12_COMMAND_LIST_TYPE_DIRECT, d3dAllocator, NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&d3dCmdList))) {
		return;
	}

	gpu_color_buffer* const cb          = gpu_get_system_color_buffer();
	gpu_depth_stencil_buffer* const dsb = gpu_get_system_depth_stencil_buffer();

	gpu_resource_barrier barrier = {};
	
	gpu_resource_barrier_clear(&barrier);
	gpu_color_buffer_enable_write(cb, &barrier);
	gpu_depth_stencil_buffer_enable_write(dsb, &barrier);

	gpu_resource_barrier_commit(&barrier, d3dCmdList);

	FLOAT clear[] = { 0, 0, 1, 0 };
	d3dCmdList->ClearRenderTargetView(cb->handle[0], clear, 0, nullptr);
	d3dCmdList->ClearDepthStencilView(dsb->handle, D3D12_CLEAR_FLAG_DEPTH | D3D12_CLEAR_FLAG_STENCIL, 1.f, 0, 0, NULL);

	FLOAT blend[] = { 1, 1, 1, 1 };

	D3D12_VIEWPORT vp;
	vp.Width = 800;
	vp.Height = 480;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
	vp.TopLeftX = 0;
	vp.TopLeftY = 0;

	D3D12_RECT scissor;
	scissor.bottom = 480;
	scissor.top = 0;
	scissor.left = 0;
	scissor.right = 800;

	d3dCmdList->RSSetViewports(1, &vp);
	d3dCmdList->RSSetScissorRects(1, &scissor);

	d3dCmdList->SetPipelineState(d3dPso);
	d3dCmdList->SetGraphicsRootSignature(d3dSignature);
	d3dCmdList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	d3dCmdList->OMSetRenderTargets(1, cb->handle, FALSE, &dsb->handle);
	d3dCmdList->OMSetStencilRef(0);
	d3dCmdList->OMSetBlendFactor(blend);
	d3dCmdList->DrawInstanced(4, 1, 0, 0);

	gpu_resource_barrier_clear(&barrier);
	gpu_color_buffer_enable_present(cb, &barrier);
	gpu_resource_barrier_commit(&barrier, d3dCmdList);

	d3dCmdList->Close();

	ID3D12CommandList* lists[] = { d3dCmdList };
	ID3D12CommandQueue* queue = gpu_get_command_queue();
	queue->ExecuteCommandLists(1, lists);


	SAFE_RELEASE(d3dCmdList);

	gpu_end_frame();
}



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPSTR lpCmdLine, INT iCmdShow)
{
	GPU_CALLBACKS callbacks;
	callbacks.deinit = frame_deinit;
	callbacks.init = frame_init;
	callbacks.draw = draw_frame;

	INT retval = gpu_start(hInstance, &callbacks, 800, 480);


	gpu_stop();

	return retval;
}