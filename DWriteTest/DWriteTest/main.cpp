#include <Windows.h>
#include <d2d1_3.h>
#include <dwrite_3.h>
#include <strsafe.h>

// constants
static const wchar_t* ClassName = L"DWriteTestWndClass";

// program globals
static HINSTANCE Instance = nullptr;
static HWND Window = nullptr;

// direct2d
ID2D1Factory* d2dFactory = nullptr;
ID2D1HwndRenderTarget* d2dRenderTarget = nullptr;
ID2D1SolidColorBrush* d2dBrush = nullptr;

// dwrite
IDWriteFactory* dwriteFactory = nullptr;
IDWriteTextFormat* dwriteFormat = nullptr;
IDWriteTextLayout* dwriteLayout = nullptr;

template<typename T> inline void SafeRelease(T*& t_) { if (t_) { t_->Release(); t_ = nullptr; } }

class CustomTextRenderer : public IDWriteTextRenderer
{
public:
	// iunknown
	ULONG STDMETHODCALLTYPE AddRef() { return 0; }
	HRESULT STDMETHODCALLTYPE QueryInterface(REFIID riid_, void** pobject) 
	{ 
		if (riid_ == __uuidof(IUnknown))
		{
			*((IUnknown**)(pobject)) = static_cast<IUnknown*>(this);
			return S_OK;
		}

		if (riid_ == __uuidof(IDWritePixelSnapping))
		{
			*((IDWritePixelSnapping**)(pobject)) = static_cast<IDWritePixelSnapping*>(this);
			return S_OK;
		}

		if (riid_ == __uuidof(IDWriteTextRenderer*))
		{
			*((IDWriteTextRenderer**)(pobject)) = static_cast<IDWriteTextRenderer*>(this);
			return S_OK;
		}

		if (riid_ == __uuidof(IDWriteTextRenderer1))
		{
			return E_FAIL;
		}

		return E_FAIL; 
	}
	ULONG STDMETHODCALLTYPE Release() { return 0; }

	STDMETHOD(GetCurrentTransform)(
		void * clientDrawingContext,
		DWRITE_MATRIX * transform
		)
	{
		d2dRenderTarget->GetTransform(reinterpret_cast<D2D1_MATRIX_3X2_F*>(transform));
		return S_OK;
	}

	STDMETHOD(GetPixelsPerDip)(
		void* clientDrawingContext,
		FLOAT* pixelsPerDip)
	{
		float x, yUnused;

		d2dRenderTarget->GetDpi(&x, &yUnused);
		*pixelsPerDip = x / 96;

		return S_OK;
	}

	STDMETHOD(IsPixelSnappingDisabled)(
		void * clientDrawingContext,
		BOOL * isDisabled
		)
	{
		*isDisabled = FALSE;
		return S_OK;
	}

	FLOAT ConvertUnits(UINT32 u_, float upm, float pointsize)
	{
		return (u_ / upm) * pointsize;
	}

	STDMETHOD(DrawGlyphRun)(
		void * clientDrawingContext,
		FLOAT  baselineOriginX,
		FLOAT  baselineOriginY,
		DWRITE_MEASURING_MODE  measuringMode,
		const DWRITE_GLYPH_RUN * glyphRun,
		const DWRITE_GLYPH_RUN_DESCRIPTION * glyphRunDescription,
		IUnknown * clientDrawingEffect
		)
	{
		DWRITE_FONT_METRICS fm;
		DWRITE_GLYPH_METRICS* gm = new DWRITE_GLYPH_METRICS[glyphRun->glyphCount];

		IDWriteFontFace* ff = glyphRun->fontFace;
		
		ff->GetMetrics(&fm);
		FLOAT fs = dwriteFormat->GetFontSize();
		ff->GetDesignGlyphMetrics(glyphRun->glyphIndices, glyphRun->glyphCount, gm, glyphRun->isSideways);

		FLOAT xpos = baselineOriginX;

		for (UINT i = 0; i < glyphRun->glyphCount; ++i)
		{
			FLOAT advanceHeight     = ConvertUnits(gm[i].advanceHeight, (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT advanceWidth      = ConvertUnits(gm[i].advanceWidth , (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT bottomSideBearing = ConvertUnits(gm[i].bottomSideBearing, (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT leftSideBearing   = ConvertUnits(gm[i].leftSideBearing, (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT rightSideBearing  = ConvertUnits(gm[i].rightSideBearing, (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT topSideBearing    = ConvertUnits(gm[i].topSideBearing, (FLOAT)fm.designUnitsPerEm, fs);
			FLOAT verticalOriginY   = ConvertUnits(gm[i].verticalOriginY, (FLOAT)fm.designUnitsPerEm, fs);

			D2D1_RECT_F rect;
			rect.bottom = baselineOriginY + advanceHeight - verticalOriginY - bottomSideBearing;
			rect.top    = baselineOriginY - verticalOriginY + topSideBearing;
			rect.left   = xpos + leftSideBearing;
			rect.right  = xpos + advanceWidth - rightSideBearing;

			xpos += advanceWidth;
			d2dRenderTarget->DrawRectangle(rect, d2dBrush, 1.f, nullptr);

		//	break;
		}

		delete[] gm;

		return S_OK;
	}

	STDMETHOD(DrawInlineObject)(
		void * clientDrawingContext,
		FLOAT  originX,
		FLOAT  originY,
		IDWriteInlineObject * inlineObject,
		BOOL  isSideways,
		BOOL  isRightToLeft,
		IUnknown * clientDrawingEffect
		)
	{
		return S_OK;
	}

	STDMETHOD(DrawStrikethrough)(
		void * clientDrawingContext,
		FLOAT  baselineOriginX,
		FLOAT  baselineOriginY,
		const DWRITE_STRIKETHROUGH * strikethrough,
		IUnknown * clientDrawingEffect
		)
	{
		return S_OK;
	}

	STDMETHOD(DrawUnderline)(
		void * clientDrawingContext,
		FLOAT  baselineOriginX,
		FLOAT  baselineOriginY,
		const DWRITE_UNDERLINE * underline,
		IUnknown * clientDrawingEffect
		) 
	{
		return S_OK;
	}
};

static CustomTextRenderer customTextRenderer;


#define IN_CASE_INVOKE_HANDLER_FOR(x) case x: return x##_handler(hwnd, msg, wparam, lparam)
#define DEFINE_HANDLER(x) static LRESULT x##_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)

static BOOL CreateDirect2DObjects(HWND hwnd)
{
	HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory), (void**)&d2dFactory);

	if (SUCCEEDED(hr)) 
	{
		D2D1_RENDER_TARGET_PROPERTIES rtp;
		rtp.minLevel              = D2D1_FEATURE_LEVEL_DEFAULT;
		rtp.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		rtp.type                  = D2D1_RENDER_TARGET_TYPE_DEFAULT;
		rtp.usage                 = D2D1_RENDER_TARGET_USAGE_NONE;

		d2dFactory->GetDesktopDpi(&rtp.dpiX, &rtp.dpiY);

		RECT cr;
		GetClientRect(hwnd, &cr);
		D2D1_HWND_RENDER_TARGET_PROPERTIES hrtp;
		hrtp.hwnd             = hwnd;
		hrtp.pixelSize.width  = cr.right - cr.left;
		hrtp.pixelSize.height = cr.bottom - cr.top;
		hrtp.presentOptions   = D2D1_PRESENT_OPTIONS_NONE;


		hr = d2dFactory->CreateHwndRenderTarget(&rtp, &hrtp, &d2dRenderTarget);
	}

	if (SUCCEEDED(hr))
	{
		const D2D1_COLOR_F color = { 1, 1, 1, 1 };
		hr = d2dRenderTarget->CreateSolidColorBrush(color, &d2dBrush);
	}

	return SUCCEEDED(hr);
}

static void DestroyDirect2DObjects()
{
	SafeRelease(d2dBrush);
	SafeRelease(d2dRenderTarget);
	SafeRelease(d2dFactory);
}

static BOOL CreateDWriteObjects(HWND hwnd_)
{
	HRESULT hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(*dwriteFactory), (IUnknown**)&dwriteFactory);

	if (SUCCEEDED(hr)) 
	{
		hr = dwriteFactory->CreateTextFormat(L"Consolas", nullptr, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL, 64.0f, L"de-DE", &dwriteFormat);
	}

	if (SUCCEEDED(hr))
	{
		RECT cr;
		GetClientRect(hwnd_, &cr);

		const wchar_t* text = L"Hallo faquam!";
		size_t len = 0;
		StringCchLength(text, STRSAFE_MAX_CCH, &len);
		hr = dwriteFactory->CreateTextLayout(text, (UINT32)len, dwriteFormat, cr.right - cr.left, cr.bottom - cr.top, &dwriteLayout);
	}

	if (SUCCEEDED(hr))
	{
		hr = dwriteLayout->SetTextAlignment(DWRITE_TEXT_ALIGNMENT_CENTER);
	}

	if (SUCCEEDED(hr))
	{
		hr = dwriteLayout->SetParagraphAlignment(DWRITE_PARAGRAPH_ALIGNMENT_CENTER);
	}

	return SUCCEEDED(hr);
}

static void DestroyDwriteObjects()
{
	SafeRelease(dwriteLayout);
	SafeRelease(dwriteFormat);
	SafeRelease(dwriteFactory);
}

static LRESULT WM_CREATE_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (!CreateDirect2DObjects(hwnd)) {
		return -1;
	}

	if (!CreateDWriteObjects(hwnd)) {
		return -1;
	}

	return 0;
}

static LRESULT WM_SIZE_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (d2dRenderTarget)
	{
		const D2D1_SIZE_U size = 
		{ 
			LOWORD(lparam), 
			HIWORD(lparam) 
		};

		HRESULT hr = d2dRenderTarget->Resize(size);
		if (SUCCEEDED(hr))
		{
			hr = dwriteLayout->SetMaxWidth((FLOAT)size.width);
		}

		if (SUCCEEDED(hr))
		{
			hr = dwriteLayout->SetMaxHeight((FLOAT)size.height);
		}

		return SUCCEEDED(hr) ? 0 : 1;
	}

	return 0;
}

static LRESULT WM_CLOSE_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DestroyWindow(hwnd);
	return 0;
}

static LRESULT WM_PAINT_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	if (!d2dRenderTarget)
	{
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}

	PAINTSTRUCT ps;
	BeginPaint(hwnd, &ps);
	EndPaint(hwnd, &ps);
	
	const D2D1_COLOR_F color = { 0.f, 0.f, 1.f, 1.f };
	const D2D1_POINT_2F origin = { 0.f, 0.f };

	d2dRenderTarget->BeginDraw();
	d2dRenderTarget->Clear(color);

	d2dRenderTarget->DrawTextLayout(origin, dwriteLayout, d2dBrush);
	dwriteLayout->Draw(nullptr, &customTextRenderer, 0.f, 0.f);
	d2dRenderTarget->EndDraw();

	return 0;
}

static LRESULT WM_DESTROY_handler(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	DestroyDwriteObjects();
	DestroyDirect2DObjects();

	PostQuitMessage(0);
	return 0;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	IN_CASE_INVOKE_HANDLER_FOR(WM_CREATE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_SIZE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_CLOSE);
	IN_CASE_INVOKE_HANDLER_FOR(WM_PAINT);
	IN_CASE_INVOKE_HANDLER_FOR(WM_DESTROY);

	default: 
		return DefWindowProc(hwnd, msg, wparam, lparam);
	}
	
}

static BOOL Init(HINSTANCE hInstance_)
{
	WNDCLASS wc;
	wc.cbClsExtra    = 0;
	wc.cbWndExtra    = 0;
	wc.hbrBackground = (HBRUSH)GetStockObject(COLOR_WINDOW);
	wc.hCursor       = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon         = LoadIcon(nullptr, IDI_APPLICATION);
	wc.hInstance     = hInstance_;
	wc.lpfnWndProc   = &WndProc;
	wc.lpszClassName = ClassName;
	wc.lpszMenuName  = nullptr;
	wc.style         = CS_VREDRAW | CS_HREDRAW;

	if (RegisterClass(&wc)) {
		Instance = hInstance_;
		return TRUE;
	}
	else {
		return FALSE;
	}
}

static void Deinit()
{
	if (Instance) {
		UnregisterClass(ClassName, Instance);
		Instance = nullptr;
	}
}

static INT Run()
{
	Window = CreateWindow(ClassName, L"Window", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 480, nullptr, nullptr, Instance, nullptr);
	if (!Window) {
		return -1;
	}

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	MSG msg = {};
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return (INT)msg.wParam;
}


INT WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT iCmdShow)
{
	BOOL ok = Init(hInstance);

	const auto retval = Run();

	Deinit();

	return retval;
}


