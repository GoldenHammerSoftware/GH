#pragma once

#include "wincodec.h"

class GHWindowsFileFinder;

// Graphics api independent util for using WIC to load textures.
// Can be shared on dx11/12.
class GHWin32WICUtil
{
public:
	GHWin32WICUtil(const GHWindowsFileFinder& fileFinder);
	~GHWin32WICUtil(void);

	bool createTexture(const char* filename,
		void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
		bool useMipmaps,
		DXGI_FORMAT& dxFormat);

private:
	const GHWindowsFileFinder& mFileFinder;
	IWICImagingFactory* mImagingFactory;
};
