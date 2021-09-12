// Copyright Golden Hammer Software
#pragma once

#include "GHFileOpener.h"

// uses fopen to open files on platforms that allow it.
class GHFOpenFileOpener : public GHFileOpener
{
protected:
	virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;
};
