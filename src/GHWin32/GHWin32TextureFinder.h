// Copyright Golden Hammer Software
#pragma once

#include "GHTextureFinderDX11.h"

class GHFileOpener;

// look at the file paths and figure out where a texture lives so we can pass it to WIC
class GHWin32TextureFinder : public GHTextureFinderDX11
{
public:
	GHWin32TextureFinder(const GHFileOpener& fileOpener);

	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const;

private:
	const GHFileOpener& mFileOpener;
};
