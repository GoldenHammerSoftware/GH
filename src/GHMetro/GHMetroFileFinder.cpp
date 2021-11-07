// Copyright 2010 Golden Hammer Software
#include "GHMetroFileFinder.h"
#include "GHPlatform/uwp/GHMetroFileOpener.h"

GHMetroFileFinder::GHMetroFileFinder(GHMetroFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

bool GHMetroFileFinder::createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const
{
	return mFileOpener.createFilePath(filename, !lookInReadOnlyDir, ret, retSize);
}
