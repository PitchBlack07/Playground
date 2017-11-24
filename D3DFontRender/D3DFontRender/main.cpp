#define WIN32_LEAN_AND_MEAN

#include <stdlib.h>
#include <Windows.h>
#include <d3d11.h>
#include <dwrite.h>
#include <dxgi.h>
#include <float.h>

#include "ps_quad.h"
#include "vs_quad.h"

#ifndef IN_CASE_INVOKE_HANDLER_FOR
#define IN_CASE_INVOKE_HANDLER_FOR(x) case x: return x##_handler(hwnd_, msg_, wparam_, lparam_)
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if((x)) { (x)->Release(); (x) = nullptr; }
#endif

#define ASCII_BEGIN 33
#define ASCII_END 126
#define ASCII_LENGTH (ASCII_END - ASCII_BEGIN + 1)
#define FONT_SIZE 16.f

// Global constants
static const WCHAR* ClassName = L"D3DFontRenderWndClass";

// Program globals
static HBRUSH bkgbrush    = nullptr;
static HINSTANCE Instance = nullptr;
static HWND Window        = nullptr;


// DXGI globals
IDXGIFactory1* dxgiFactory    = nullptr;
IDXGIAdapter1* dxgiAdapter    = nullptr;
IDXGISwapChain* dxgiSwapChain = nullptr;

// D3D11 globals
ID3D11Device* d3dDevice                 = nullptr;
ID3D11DeviceContext* d3dDeviceContext   = nullptr;
ID3D11RenderTargetView* d3dRenderTarget = nullptr;
ID3D11Buffer* d3dQuad                   = nullptr;
ID3D11VertexShader* d3dQuadVS           = nullptr;
ID3D11PixelShader* d3dQuadPS            = nullptr;
ID3D11InputLayout* d3dQuadLayout        = nullptr;
ID3D11ShaderResourceView* d3dGlyphTexture = nullptr;
ID3D11SamplerState* d3dGlyphSampler = nullptr;
ID3D11BlendState* d3dGlyphBlend = nullptr;
ID3D11Buffer* d3dGlyphAddressTable = nullptr;

// DWrite globals
IDWriteFactory* dwriteFactory  = nullptr;
IDWriteFont* dwriteFont        = nullptr;
struct QuadVertex
{
	FLOAT x;
	FLOAT y;
	UINT  idx;
};

struct GlyphImage
{
	UINT16 Width;
	UINT16 Height;
	BYTE*  Values;
};

struct GlyphAddress
{
	FLOAT xmin, ymin;
	FLOAT xmax, ymax;
};

static GlyphAddress GlyphAddressTable[ASCII_LENGTH];

static LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_);

static BOOL CreateDirectWriteObjects()
{
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_ISOLATED, __uuidof(IDWriteFactory), reinterpret_cast<IUnknown**>(&dwriteFactory));
	IDWriteFontCollection* dwriteFontCollection = nullptr;

	if (SUCCEEDED(hr))
	{
		hr= dwriteFactory->GetSystemFontCollection(&dwriteFontCollection);
	}

	UINT32 fontIndex = -1;
	BOOL fontExists  = FALSE;

	if (SUCCEEDED(hr))
	{
		hr = dwriteFontCollection->FindFamilyName(L"Arial", &fontIndex, &fontExists);
	}

	IDWriteFontFamily* fontFamily = nullptr;
	if (fontExists)
	{
		hr = dwriteFontCollection->GetFontFamily(fontIndex, &fontFamily);
	}
	else
		hr = E_FAIL;

	if (SUCCEEDED(hr))
	{
		hr = fontFamily->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL, DWRITE_FONT_STYLE_NORMAL, &dwriteFont);
	}

	SAFE_RELEASE(fontFamily);
	SAFE_RELEASE(dwriteFontCollection);

	return SUCCEEDED(hr);
}

static void DestroyDirectWriteObjects()
{
	SAFE_RELEASE(dwriteFont);
	SAFE_RELEASE(dwriteFactory);
}

static BOOL Init(HINSTANCE hInstance_)
{
	if (!CreateDirectWriteObjects())
		return FALSE;

	bkgbrush = CreateSolidBrush(RGB(0, 0, 255));
	if (!bkgbrush)
		return FALSE;

	WNDCLASS wc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = bkgbrush;
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance     = hInstance_;
	wc.lpfnWndProc   = WndProc;
	wc.lpszClassName = ClassName;
	wc.lpszMenuName  = nullptr;
	wc.style         = CS_VREDRAW | CS_HREDRAW;

	if (RegisterClass(&wc))
	{
		Instance = hInstance_;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

static void Deinit()
{
	DestroyDirectWriteObjects();

	if (bkgbrush)
	{
		DeleteObject(bkgbrush);
	}

	if (Instance)
	{
		UnregisterClass(ClassName, Instance);
		Instance = nullptr;
	}
}

static BOOL CreateDXGIObjects()
{
	HRESULT hr = CreateDXGIFactory1(__uuidof(*dxgiFactory), reinterpret_cast<void**>(&dxgiFactory));

	if (SUCCEEDED(hr))
	{
		hr = dxgiFactory->EnumAdapters1(0, &dxgiAdapter);
	}

	return SUCCEEDED(hr);
}

static void DestroyDXGIObject()
{
	SAFE_RELEASE(dxgiAdapter);
	SAFE_RELEASE(dxgiFactory);
}

static BOOL CreateD3D11Shaders()
{
	HRESULT hr = d3dDevice->CreateVertexShader(vs_quad, sizeof(vs_quad), nullptr, &d3dQuadVS);

	if (SUCCEEDED(hr))
	{
		hr = d3dDevice->CreatePixelShader(ps_quad, sizeof(ps_quad), nullptr, &d3dQuadPS);
	}

	if (SUCCEEDED(hr))
	{
		const D3D11_INPUT_ELEMENT_DESC iadesc[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0                , D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32_UINT    , 0, 2 * sizeof(FLOAT), D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};

		hr = d3dDevice->CreateInputLayout(iadesc, _countof(iadesc), vs_quad, sizeof(vs_quad), &d3dQuadLayout);
	}

	return SUCCEEDED(hr);
}

static void DestroyD3D11Shader()
{
	SAFE_RELEASE(d3dQuadLayout);
	SAFE_RELEASE(d3dQuadVS);
	SAFE_RELEASE(d3dQuadPS);
}

static BOOL CreateD3D11Objects()
{
	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	const D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;
	D3D_FEATURE_LEVEL selectedFeatureLevel;
	HRESULT hr = D3D11CreateDevice(dxgiAdapter, D3D_DRIVER_TYPE_UNKNOWN, nullptr, flags, &featureLevel, 1, D3D11_SDK_VERSION, &d3dDevice, &selectedFeatureLevel, &d3dDeviceContext);

	QuadVertex quad[] = 
	{
		{ -1.f, -1.f, 55 },
		{ -1.f, 1.f , 55 },
		{ 1.f, -1.f , 55 },
		{ 1.f, 1.f  , 55 }
	};

	D3D11_BUFFER_DESC bd;
	bd.BindFlags           = D3D11_BIND_VERTEX_BUFFER;
	bd.ByteWidth           = sizeof(quad);
	bd.CPUAccessFlags      = 0;
	bd.MiscFlags           = 0;
	bd.StructureByteStride = 0;
	bd.Usage               = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem          = quad;
	sd.SysMemPitch      = sizeof(quad);
	sd.SysMemSlicePitch = sizeof(quad);

	hr = d3dDevice->CreateBuffer(&bd, &sd, &d3dQuad);

	if (SUCCEEDED(hr))
	{
		D3D11_SAMPLER_DESC sd = {};
		sd.AddressU       = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressV       = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.AddressW       = D3D11_TEXTURE_ADDRESS_CLAMP;
		sd.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sd.Filter         = D3D11_FILTER_MIN_MAG_MIP_POINT;
		sd.MaxLOD         = FLT_MAX;
		sd.MinLOD         = 0.f;
		sd.MipLODBias     = 0.f;

		hr = d3dDevice->CreateSamplerState(&sd, &d3dGlyphSampler);
	}

	if (SUCCEEDED(hr))
	{
		D3D11_BLEND_DESC blenddesc;

		blenddesc.AlphaToCoverageEnable = FALSE;
		blenddesc.IndependentBlendEnable = FALSE;

		for (UINT i = 0; i < _countof(blenddesc.RenderTarget); ++i)
		{
			D3D11_RENDER_TARGET_BLEND_DESC& rtbd = blenddesc.RenderTarget[i];
			rtbd.BlendEnable = TRUE;
			rtbd.BlendOp = D3D11_BLEND_OP_ADD;
			rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbd.DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
			rtbd.SrcBlend = D3D11_BLEND_ONE;
			rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtbd.RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		}
		hr = d3dDevice->CreateBlendState(&blenddesc, &d3dGlyphBlend);
	}

	return SUCCEEDED(hr);
}

static void DestroyD3D11Objects()
{
	SAFE_RELEASE(d3dGlyphBlend);
	SAFE_RELEASE(d3dGlyphSampler);
	SAFE_RELEASE(d3dQuad);
	SAFE_RELEASE(d3dDeviceContext);
	SAFE_RELEASE(d3dDevice);
}

static BOOL CreateDXGISwapChain(HWND window_)
{
	RECT cr;
	GetClientRect(window_, &cr);

	DXGI_SWAP_CHAIN_DESC scd;
	scd.BufferCount = 3;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.Height = cr.bottom - cr.top;
	scd.BufferDesc.Width = cr.right - cr.top;
	scd.BufferDesc.RefreshRate.Denominator = 0;
	scd.BufferDesc.RefreshRate.Numerator = 0;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_BACK_BUFFER | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = 0;
	scd.OutputWindow = window_;
	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Windowed = TRUE;
	
	HRESULT hr = dxgiFactory->CreateSwapChain(d3dDevice, &scd, &dxgiSwapChain);

	return SUCCEEDED(hr);
}

static void DestroyDXGISwapChain()
{
	SAFE_RELEASE(d3dRenderTarget);
	SAFE_RELEASE(dxgiSwapChain);
}

static FLOAT GetFontEmSize(FLOAT fontPointSize_)
{
	return fontPointSize_ / 72.f * 96.f;
}

static int GlyphImageCompareByHeight(const void* a, const void* b)
{
	const GlyphImage* const* aa = static_cast<const GlyphImage* const*>(a);
	const GlyphImage* const* bb = static_cast<const GlyphImage* const*>(b);

	int diff = -static_cast<int>((*aa)->Height - (*bb)->Height);
	if (diff == 0)
	{
		diff = -static_cast<int>((*aa)->Width - (*bb)->Width);
	}
	return diff;
}

static BOOL CreateGlyphTexture()
{
	BOOL retval = FALSE;
	
	BYTE* bitmap = nullptr;
	BYTE* d3dBitmap = nullptr;
	ID3D11Texture2D* tex2d = nullptr;
	IDWriteGlyphRunAnalysis* analysis = nullptr;
	IDWriteFontFace* fontFace         = nullptr;

	GlyphImage glyphImages[ASCII_LENGTH];
	GlyphImage* imageptr[ASCII_LENGTH];

	memset(&glyphImages, 0, sizeof(glyphImages));
	for (UINT i = 0; i < _countof(imageptr); ++i) imageptr[i] = &glyphImages[i];

	dwriteFont->CreateFontFace(&fontFace);

	HRESULT hr = E_FAIL;

	for (UINT i = 0; i < _countof(glyphImages); ++i)
	{
		DWRITE_GLYPH_RUN run   = {};
		DWRITE_GLYPH_OFFSET go = {};
		FLOAT ga               = 0.f;
		UINT32 cp              = i + ASCII_BEGIN;
		UINT16 gi              = 0;

		hr = fontFace->GetGlyphIndicesW(&cp, 1, &gi);

		if (FAILED(hr)) goto cleanup;

		run.bidiLevel     = 0;
		run.fontEmSize    = GetFontEmSize(FONT_SIZE);
		run.fontFace      = fontFace;
		run.glyphAdvances = &ga;
		run.glyphIndices  = &gi;
		run.glyphCount    = 1;
		run.glyphOffsets  = &go;
		run.isSideways    = FALSE;

		hr = dwriteFactory->CreateGlyphRunAnalysis(&run, 1.f, nullptr, DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL, DWRITE_MEASURING_MODE_NATURAL, 0.f, 0.f, &analysis);

		if (FAILED(hr)) {
			goto cleanup;
		}

		RECT tb;
		hr = analysis->GetAlphaTextureBounds(DWRITE_TEXTURE_CLEARTYPE_3x1, &tb);
		if (FAILED(hr)) {
			goto cleanup;
		}

		const UINT width  = tb.right - tb.left;
		const UINT height = tb.bottom - tb.top;

		bitmap = (BYTE*)malloc(3 * width * height);
		if (!bitmap) {
			goto cleanup;
		}

		hr = analysis->CreateAlphaTexture(DWRITE_TEXTURE_CLEARTYPE_3x1, &tb, bitmap, 3 * width * height);
		if (FAILED(hr)) {
			goto cleanup;
		}

		glyphImages[i].Values = (BYTE*)malloc(width * height);
		if(!glyphImages[i].Values) {
			goto cleanup;
		}
		
		for (UINT j = 0; j < width * height; ++j)
		{
			INT l =
				299 * bitmap[3 * j + 0] +
				587 * bitmap[3 * j + 1] +
				114 * bitmap[3 * j + 2];

			l /= 1000;

			glyphImages[i].Values[j] = static_cast<BYTE>(l);
		}
		free(bitmap);
		bitmap = nullptr;

		glyphImages[i].Width  = width;
		glyphImages[i].Height = height;

	} // end for

	//qsort(imageptr, _countof(imageptr), sizeof(imageptr[0]), GlyphImageCompareByHeight);

	UINT twidth = 256;
	UINT theight = 128;

	BYTE* tex = (BYTE*)malloc(twidth * theight);
	memset(tex, 0, twidth * theight);
	UINT y = 0;
	UINT x = 0;
	UINT g = 0;
	while(y < theight)
	{
		GlyphImage* gi = imageptr[g];

		UINT dy = 0;
		while (gi->Width + x < twidth)
		{
			GlyphAddressTable[g].xmin = x / static_cast<float>(twidth);
			GlyphAddressTable[g].ymin = y / static_cast<float>(theight);
			GlyphAddressTable[g].xmax = (x + gi->Width) / static_cast<float>(twidth);
			GlyphAddressTable[g].ymax = (y + gi->Height) / static_cast<float>(theight);

			for (UINT k = 0; k < gi->Height; ++k)
			{
				memcpy(&tex[(y+k) * twidth + x], &gi->Values[k * gi->Width], gi->Width);
			}

			dy = dy < gi->Height ? gi->Height : dy;
			x += gi->Width;
			if (g < ASCII_LENGTH - 1)
				gi = imageptr[++g];
			else
				goto end_loop;
		}
		x   = 0;
		y += dy;

	}

end_loop:
	D3D11_TEXTURE2D_DESC td;
	td.ArraySize = 1;
	td.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	td.CPUAccessFlags = 0;
	td.Format = DXGI_FORMAT_R8_UNORM;
	td.Height = theight;
	td.Width = twidth;
	td.MipLevels = 1;
	td.MiscFlags = 0;
	td.SampleDesc.Count = 1;
	td.SampleDesc.Quality = 0;
	td.Usage = D3D11_USAGE_IMMUTABLE;

	D3D11_SUBRESOURCE_DATA sd;
	sd.pSysMem = tex;
	sd.SysMemPitch = twidth;
	sd.SysMemSlicePitch = twidth * theight;

	hr = d3dDevice->CreateTexture2D(&td, &sd, &tex2d);
	if (FAILED(hr)) goto cleanup;

	D3D11_SHADER_RESOURCE_VIEW_DESC srvd;
	srvd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	srvd.Format = td.Format;
	srvd.Texture2D.MipLevels = 1;
	srvd.Texture2D.MostDetailedMip = 0;

	hr = d3dDevice->CreateShaderResourceView(tex2d, &srvd, &d3dGlyphTexture);
	if (FAILED(hr)) goto cleanup;

	D3D11_BUFFER_DESC gatdesc;
	gatdesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	gatdesc.ByteWidth = sizeof(GlyphAddressTable);
	gatdesc.CPUAccessFlags = 0;
	gatdesc.MiscFlags = 0;
	gatdesc.StructureByteStride = 0;
	gatdesc.Usage = D3D11_USAGE_IMMUTABLE;

	sd.pSysMem = GlyphAddressTable;
	sd.SysMemPitch = sizeof(GlyphAddressTable);
	sd.SysMemSlicePitch = 0;

	hr = d3dDevice->CreateBuffer(&gatdesc, &sd, &d3dGlyphAddressTable);

	retval = TRUE;

cleanup:
	if (tex) {
		free(tex); tex = nullptr;
	}
	SAFE_RELEASE(tex2d);
	for (UINT i = 0; i < _countof(glyphImages); ++i) { free(glyphImages[i].Values); glyphImages[i].Values = nullptr; }
	if(bitmap) free(bitmap);
	SAFE_RELEASE(analysis);
	SAFE_RELEASE(fontFace);
	return retval;
}

static void DestroyGlyphTexture()
{
	SAFE_RELEASE(d3dGlyphAddressTable);
	SAFE_RELEASE(d3dGlyphTexture);
}

static LRESULT WM_CREATE_handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	if (!CreateDXGIObjects()) 
	{
		return -1;
	}

	if (!CreateD3D11Objects())
	{
		return -1;
	}

	if (!CreateDXGISwapChain(hwnd_))
	{
		return -1;
	}

	if (!CreateD3D11Shaders())
	{
		return -1;
	}

	if (!CreateGlyphTexture()) 
	{
		return -1;
	}

	return 0;
}

static LRESULT WM_SIZE_handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	HRESULT hr = S_OK;

	if (dxgiSwapChain)
	{
		UINT width  = LOWORD(lparam_);
		UINT height = HIWORD(lparam_);
		
		SAFE_RELEASE(d3dRenderTarget);
		DXGI_SWAP_CHAIN_DESC scd;
		dxgiSwapChain->GetDesc(&scd);

		hr = dxgiSwapChain->ResizeBuffers(scd.BufferCount, width, height, scd.BufferDesc.Format, scd.Flags);

		D3D11_RENDER_TARGET_VIEW_DESC rtvd;
		rtvd.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2D;
		rtvd.Texture2D.MipSlice = 0;
		rtvd.Format             = scd.BufferDesc.Format;

		ID3D11Texture2D* tex2d = nullptr;
		dxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&tex2d);
		hr = d3dDevice->CreateRenderTargetView(tex2d, &rtvd, &d3dRenderTarget);
		SAFE_RELEASE(tex2d);
	}

	return SUCCEEDED(hr) ? 0 : -1;
}

static LRESULT WM_DESTROY_handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	DestroyGlyphTexture();
	DestroyD3D11Shader();
	DestroyDXGISwapChain();
	DestroyD3D11Objects();
	DestroyDXGIObject();

	PostQuitMessage(0);
	return 0;
}

static LRESULT WM_CLOSE_handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	DestroyWindow(hwnd_);
	return 0;
}

static LRESULT WM_PAINT_handler(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	RECT rect;
	GetClientRect(hwnd_, &rect);
	PAINTSTRUCT ps;
	BeginPaint(hwnd_, &ps);
	EndPaint(hwnd_, &ps);

	ID3D11Buffer* vsbuffer[] = { d3dQuad };
	UINT offset[] = { 0 };
	UINT stride[] = { sizeof(QuadVertex) };

	FLOAT color[] = { 0, 1, 0, 1 };
	FLOAT blendfactor[] = { 0,0,0,0 };
	D3D11_VIEWPORT vp;
	vp.Height = static_cast<FLOAT>(rect.bottom - rect.top);
	vp.Width = static_cast<FLOAT>(rect.right - rect.left);
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;

	d3dDeviceContext->RSSetViewports(1, &vp);
	d3dDeviceContext->ClearRenderTargetView(d3dRenderTarget, color);
	d3dDeviceContext->OMSetRenderTargets(1, &d3dRenderTarget, nullptr);
	d3dDeviceContext->IASetInputLayout(d3dQuadLayout);
	
	d3dDeviceContext->IASetVertexBuffers(0, 1, vsbuffer, stride, offset);
	d3dDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	d3dDeviceContext->VSSetShader(d3dQuadVS, nullptr, 0);
	d3dDeviceContext->VSSetConstantBuffers(0, 1, &d3dGlyphAddressTable);
	d3dDeviceContext->PSSetShader(d3dQuadPS, nullptr, 0);
	d3dDeviceContext->PSSetSamplers(0, 1, &d3dGlyphSampler);
	d3dDeviceContext->PSSetShaderResources(0, 1, &d3dGlyphTexture);
	d3dDeviceContext->OMSetBlendState(d3dGlyphBlend, blendfactor, 0xFFFFFFFF);

	d3dDeviceContext->Draw(4, 0);
	dxgiSwapChain->Present(0, 0);
	
	return 0;
}


LRESULT CALLBACK WndProc(HWND hwnd_, UINT msg_, WPARAM wparam_, LPARAM lparam_)
{
	switch (msg_)
	{
	IN_CASE_INVOKE_HANDLER_FOR(WM_CREATE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_SIZE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_PAINT);
	IN_CASE_INVOKE_HANDLER_FOR(WM_CLOSE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_DESTROY);

	default:
		return DefWindowProc(hwnd_, msg_, wparam_, lparam_);
	}
}


static INT RunApplication()
{
	RECT cr = { 0, 0, 800, 480 };
	AdjustWindowRect(&cr, WS_OVERLAPPEDWINDOW, FALSE);

	Window = CreateWindow(ClassName, L"D3DFontApp", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, cr.right - cr.left, cr.bottom - cr.top, nullptr, nullptr, Instance, nullptr);

	if (Window)
	{
		ShowWindow(Window, SW_SHOW);

		MSG msg = {};
		while (GetMessage(&msg, nullptr, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		return (INT)msg.wParam;
	}
	return -1;
}

INT WINAPI WinMain(HINSTANCE hInstance_, HINSTANCE hPrevInstance_, LPSTR lpCmdLine_, INT iShow_)
{
	
	if (!Init(hInstance_)) {
		return -1;
	}

	const INT retval = RunApplication();

	Deinit();

	return retval;
}
