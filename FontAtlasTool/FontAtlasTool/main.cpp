#include <stdlib.h>
#include <stdint.h>
#include <Windows.h>
#include <dwrite.h>
#include <tchar.h>
#include <strsafe.h>
#include <assert.h>

// macros
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(x) if((x)) { (x)->Release(); (x) = nullptr; }
#endif

struct FontRasterImage
{
	UINT16 Width;
	UINT16 Height;
	BYTE*  Values;
};

struct FontFileEntry
{
	UINT16 Offset;
	UINT16 Width;
	UINT16 Height;
};

struct FontFile
{
	const wchar_t* FontName;
	FLOAT          FontSize;

	DWRITE_FONT_WEIGHT  FontWeight;
	DWRITE_FONT_STYLE   FontStyle;
	DWRITE_FONT_STRETCH FontStretch;

	FontFileEntry Table[126 - 33 + 1];
};

// command line params
static const TCHAR* FontName           = TEXT("Arial");
static FLOAT        FontSize           = 32.f;
static DWRITE_FONT_WEIGHT FontWeight   = DWRITE_FONT_WEIGHT_NORMAL;
static DWRITE_FONT_STYLE FontStyle     = DWRITE_FONT_STYLE_NORMAL;
static DWRITE_FONT_STRETCH FontStretch = DWRITE_FONT_STRETCH_NORMAL;
static const TCHAR* FontLocale         = TEXT("de-DE");
static UINT         AtlasWidth         = 256;
static UINT         AtlasHeight        = 256;
static const UINT   AsciiBegin         = 33; // -> Char '!'
static const UINT   AsciiEnd           = 126;
static TCHAR        AsciiString[AsciiEnd - AsciiBegin + 2];
static FontRasterImage FontRasterImages[AsciiEnd - AsciiBegin + 1];

// DirectWrite Objects
static IDWriteFactory* dwriteFactory   = nullptr;
static IDWriteFont* dwriteFont         = nullptr;
static IDWriteFontFace* dwriteFontFace = nullptr;

static void LoadAsciiString()
{
	TCHAR* str = AsciiString;

	for (UINT i = AsciiBegin; i <= AsciiEnd; ++i, ++str) {
		*str = (TCHAR)i;
	}
	*str = 0;
}

static inline FLOAT ConvertFontSize2Dip(FLOAT fontsize_)
{
	return fontsize_ / 72.f * 96.f;
}

static BOOL LoadCommandLineParameters(INT argc_, const TCHAR* const* argv_) 
{
	return TRUE;
}

static BOOL CreateDirectWriteObjects()
{
	HRESULT hr             = E_FAIL;
	FLOAT dipFontSize      = ConvertFontSize2Dip(FontSize);
	UINT32 fontFamilyIndex = 0;
	BOOL fontFamilyExists  = FALSE;

	IDWriteFontCollection* dwriteFontCollection = nullptr;
	IDWriteFontFamily* dwriteFontFamily         = nullptr;

	hr = dwriteFactory->GetSystemFontCollection(&dwriteFontCollection);

	if (SUCCEEDED(hr)) 
	{
		const HRESULT findOk = dwriteFontCollection->FindFamilyName(FontName, &fontFamilyIndex, &fontFamilyExists);
		hr = fontFamilyExists ? findOk : E_FAIL;
	}

	if (SUCCEEDED(hr))
	{
		hr = dwriteFontCollection->GetFontFamily(fontFamilyIndex, &dwriteFontFamily);
	}

	if (SUCCEEDED(hr))
	{
		hr = dwriteFontFamily->GetFirstMatchingFont(FontWeight, FontStretch, FontStyle, &dwriteFont);
	}
	
	if (SUCCEEDED(hr))
	{
		hr = dwriteFont->CreateFontFace(&dwriteFontFace);
	}

	SAFE_RELEASE(dwriteFontFamily);
	SAFE_RELEASE(dwriteFontCollection);

	return SUCCEEDED(hr);
}

static void DestroyDirectWriteObjects()
{
	SAFE_RELEASE(dwriteFontFace);
	SAFE_RELEASE(dwriteFont);
}

static BOOL Init(INT argc_, const TCHAR* const* argv_)
{
	BOOL success = LoadCommandLineParameters(argc_, argv_);

	if (success)
	{
		LoadAsciiString();

		success = SUCCEEDED(
			DWriteCreateFactory(
				DWRITE_FACTORY_TYPE_ISOLATED, 
				__uuidof(*dwriteFactory), 
				reinterpret_cast<IUnknown**>(&dwriteFactory)
			)
		);
	}

	if (success)
	{
		success = CreateDirectWriteObjects();
	}

	return success;
}

static void Deinit()
{
	for (UINT i = 0; i < _countof(FontRasterImages); ++i) { free(FontRasterImages[i].Values); }
	memset(FontRasterImages, 0, sizeof(FontRasterImages));

	DestroyDirectWriteObjects();
	SAFE_RELEASE(dwriteFactory);
}

static BOOL RasterizeCharacters()
{
	UINT16 glyphIds[AsciiEnd - AsciiBegin + 1];
	UINT32 codePoints[_countof(glyphIds)];

	for (UINT i = 0; i < _countof(codePoints); ++i)
	{
		codePoints[i] = i + AsciiBegin;
	}

	HRESULT hr = dwriteFontFace->GetGlyphIndicesW(codePoints, _countof(codePoints), glyphIds);

	if (SUCCEEDED(hr))
	{
		const FLOAT fontSizeEm           = ConvertFontSize2Dip(FontSize);
		const FLOAT advance              = 0.f;
		const DWRITE_GLYPH_OFFSET offset = {};

		for (UINT i = 0; i < _countof(glyphIds); ++i)
		{
			DWRITE_GLYPH_RUN grun;
			IDWriteGlyphRunAnalysis* glyphAnalysis = nullptr;

			grun.bidiLevel     = 0;
			grun.fontEmSize    = fontSizeEm;
			grun.fontFace      = dwriteFontFace;
			grun.glyphAdvances = &advance;
			grun.glyphIndices  = glyphIds + i;
			grun.glyphOffsets  = &offset;
			grun.isSideways    = FALSE;
			grun.glyphCount    = 1;

			hr = dwriteFactory->CreateGlyphRunAnalysis(&grun, 1.f, nullptr, DWRITE_RENDERING_MODE_CLEARTYPE_NATURAL, DWRITE_MEASURING_MODE_NATURAL, 0.f, 0.f, &glyphAnalysis);

			if (SUCCEEDED(hr))
			{
				RECT gr;
				glyphAnalysis->GetAlphaTextureBounds(DWRITE_TEXTURE_CLEARTYPE_3x1, &gr);

				const UINT width  = gr.right - gr.left;
				const UINT height = gr.bottom - gr.top;

				BYTE* const mem = (BYTE*)malloc(width * height * 3);

				glyphAnalysis->CreateAlphaTexture(DWRITE_TEXTURE_CLEARTYPE_3x1, &gr, mem, width * height * 3);

				FontRasterImages[i].Width  = width;
				FontRasterImages[i].Height = height;
				FontRasterImages[i].Values = mem;
			}

			SAFE_RELEASE(glyphAnalysis);
		}
	}

	return SUCCEEDED(hr);
}

INT _tmain(INT argc_, TCHAR** argv_)
{
	BOOL success = Init(argc_, argv_);
	if (success)
	{
		success = RasterizeCharacters();
	}


	Deinit();
	return success ? 0 : -1;
}
