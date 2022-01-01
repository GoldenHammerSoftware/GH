#pragma once

#include "dxgiformat.h"
#include "Render/GHTextureFormat.h"

namespace GHDXGIUtil
{
    DXGI_FORMAT convertGHFormatToDXGI(GHTextureFormat::Enum ghFormat);
    size_t bitsPerPixel(DXGI_FORMAT fmt);

	// from microsoft.
	// http://go.microsoft.com/fwlink/?LinkId=248926
	// http://go.microsoft.com/fwlink/?LinkId=248929
	void getSurfaceInfo(_In_ size_t width,
		_In_ size_t height,
		_In_ DXGI_FORMAT fmt,
		_Out_opt_ size_t* outNumBytes,
		_Out_opt_ size_t* outRowBytes,
		_Out_opt_ size_t* outNumRows);

};
