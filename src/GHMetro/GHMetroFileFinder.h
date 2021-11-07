// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHWin32/GHWindowsFileFinder.h"

class GHMetroFileOpener;

// interface for finding out where a texture lives so we can pass it to the os texture load functions.
class GHMetroFileFinder : public GHWindowsFileFinder
{
public:
	GHMetroFileFinder(GHMetroFileOpener& fileOpener);

	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const;

private:
	GHMetroFileOpener& mFileOpener;
};
