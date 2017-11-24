#include <wincodec.h>
#include <Windows.h>
#include <d2d1_3.h>

static IWICImagingFactory *piFactory = nullptr;
static ID2D1Factory3* d2dFactory     = nullptr;

struct Pixel
{
	BYTE b;
	BYTE g;
	BYTE r;
	BYTE a;
};

template<typename T>
inline void SafeRelease(T*& t_) { if (t_) t_->Release(); t_ = nullptr; }

static void WriteBitmapToFile(LPCTSTR filename_, UINT width_, UINT height_, Pixel* pixels_)
{
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;

	IWICStream *piStream = NULL;
	UINT uiWidth = 640;
	UINT uiHeight = 480;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateStream(&piStream);
	}

	if (SUCCEEDED(hr))
	{
		hr = piStream->InitializeFromFilename(filename_, GENERIC_WRITE);
	}

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->CreateNewFrame(&piBitmapFrame, nullptr);
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->Initialize(nullptr);
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->SetSize(uiWidth, uiHeight);
	}

	WICPixelFormatGUID formatGUID = GUID_WICPixelFormat32bppPBGRA;
	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->SetPixelFormat(&formatGUID);
	}

	if (SUCCEEDED(hr))
	{
		// We're expecting to write out 24bppRGB. Fail if the encoder cannot do it.
		hr = IsEqualGUID(formatGUID, GUID_WICPixelFormat32bppPBGRA) ? S_OK : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		UINT cbStride = (uiWidth * 32 + 7) / 8/***WICGetStride***/;
		UINT cbBufferSize = uiHeight * cbStride;

		BYTE *pbBuffer = (BYTE*)pixels_;

		if (pbBuffer != NULL)
		{
			hr = piBitmapFrame->WritePixels(uiHeight, cbStride, cbBufferSize, pbBuffer);
		}
		else
		{
			hr = E_OUTOFMEMORY;
		}
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->Commit();
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Commit();
	}

	if (piBitmapFrame)
		piBitmapFrame->Release();

	if (piEncoder)
		piEncoder->Release();

	if (piStream)
		piStream->Release();
}

static void WriteBitmapToFile(LPCTSTR filename_, IWICBitmap* bitmap)
{
	IWICBitmapEncoder *piEncoder = NULL;
	IWICBitmapFrameEncode *piBitmapFrame = NULL;

	IWICStream *piStream = NULL;

	HRESULT hr = S_OK;

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateStream(&piStream);
	}

	if (SUCCEEDED(hr))
	{
		hr = piStream->InitializeFromFilename(filename_, GENERIC_WRITE);
	}

	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateEncoder(GUID_ContainerFormatBmp, NULL, &piEncoder);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Initialize(piStream, WICBitmapEncoderNoCache);
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->CreateNewFrame(&piBitmapFrame, nullptr);
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->Initialize(nullptr);
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->WriteSource(bitmap, nullptr);
	}

	if (SUCCEEDED(hr))
	{
		hr = piBitmapFrame->Commit();
	}

	if (SUCCEEDED(hr))
	{
		hr = piEncoder->Commit();
	}

	if (piBitmapFrame)
		piBitmapFrame->Release();

	if (piEncoder)
		piEncoder->Release();

	if (piStream)
		piStream->Release();
}

static HRESULT WICInit()
{
	return CoCreateInstance(
		CLSID_WICImagingFactory,
		NULL,
		CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory,
		(LPVOID*)&piFactory);
}

static HRESULT D2DInit()
{
	D2D1_FACTORY_OPTIONS options;
	options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
	return D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, options, &d2dFactory);
}

static void D2DDeinit()
{
	SafeRelease(d2dFactory);
}

int main(int argc, char** argv)
{
	HRESULT hr = CoInitialize(nullptr);
	int retval = -1;

	if (SUCCEEDED(hr))
	{
		hr = WICInit();
	}

	if (SUCCEEDED(hr))
	{
		hr = D2DInit();
	}

	IWICBitmap* bitmap = nullptr;
	if (SUCCEEDED(hr))
	{
		hr = piFactory->CreateBitmap(64, 64, GUID_WICPixelFormat32bppPBGRA, WICBitmapCacheOnDemand, &bitmap);
	}

	if (SUCCEEDED(hr))
	{
		D2D1_RENDER_TARGET_PROPERTIES rtp;
		rtp.type                  = D2D1_RENDER_TARGET_TYPE_SOFTWARE;
		rtp.pixelFormat.format    = DXGI_FORMAT_B8G8R8A8_UNORM;
		rtp.pixelFormat.alphaMode = D2D1_ALPHA_MODE_PREMULTIPLIED;
		rtp.dpiX                  = 96.f;
		rtp.dpiY                  = 96.f;
		rtp.usage                 = D2D1_RENDER_TARGET_USAGE_NONE;
		rtp.minLevel              = D2D1_FEATURE_LEVEL_DEFAULT;

		ID2D1RenderTarget* rt = nullptr;
		hr = d2dFactory->CreateWicBitmapRenderTarget(bitmap, &rtp,  &rt);

		D2D1_COLOR_F color = { 1.f, 0.f, 0.f, 1.f };
		rt->BeginDraw();
		rt->Clear(color);
		rt->Flush();
		rt->EndDraw();

		SafeRelease(rt);

		WriteBitmapToFile(L"output.bmp", bitmap);
	}

	

	SafeRelease(bitmap);
	SafeRelease(piFactory);
	D2DDeinit();

	return 0;
}
