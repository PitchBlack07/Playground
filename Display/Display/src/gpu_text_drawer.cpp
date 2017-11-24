#include "gpu_text_drawer.h"
#include "utils.h"
#include <dwrite.h>

static IDWriteFactory* dwriteFactory = nullptr;

BOOL gpu_text_drawer_create()
{
	if (FAILED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), (IUnknown**)&dwriteFactory))) {
		return FALSE;
	}

	return FALSE;
}

void gpu_text_drawer_destroy()
{
	SAFE_RELEASE(dwriteFactory);
}

void gpu_draw_text(ID3D12GraphicsCommandList* cmdlist_, INT32 x_, INT32 y_, LPCTSTR text_)
{

}