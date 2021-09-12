// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTextureFinderDX11.h"

class GHMetroFileOpener;

// interface for finding out where a texture lives so we can pass it to the os texture load functions.
class GHMetroTextureFinder : public GHTextureFinderDX11
{
public:
	GHMetroTextureFinder(GHMetroFileOpener& fileOpener);

	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const;

private:
	GHMetroFileOpener& mFileOpener;
};
