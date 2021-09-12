// Copyright 2010 Golden Hammer Software
#include "GHMetroTextureFinder.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"

GHMetroTextureFinder::GHMetroTextureFinder(GHMetroFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

bool GHMetroTextureFinder::createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const
{
	return mFileOpener.createFilePath(filename, !lookInReadOnlyDir, ret, retSize);
}
