// Copyright Golden Hammer Software
#pragma once

#include "GHWindowsFileFinder.h"

class GHFileOpener;

// look at the file paths and figure out where a texture lives so we can pass it to WIC
class GHWin32FileFinder : public GHWindowsFileFinder
{
public:
	GHWin32FileFinder(const GHFileOpener& fileOpener);

	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const;
	bool fileExists(const char* filename) const;

private:
	const GHFileOpener& mFileOpener;
};
