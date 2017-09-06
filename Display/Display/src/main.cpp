#include <Windows.h>
#include "gpu.h"
#include "utils.h"
#include <d3d12.h>

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

static INT app_run()
{
	MSG msg           = {};

	while (msg.message != WM_QUIT)
	{
		draw_frame();
		
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT) 
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	return (INT)msg.lParam;
}



INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevIntance, LPSTR lpCmdLine, INT iCmdShow)
{
	INT retval = 0;

	gpu_start(hInstance, 800, 480);

	retval = app_run();

	gpu_stop();

	return retval;
}