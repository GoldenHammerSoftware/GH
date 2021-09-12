// Copyright Golden Hammer Software
#include "GHFOpenFileOpener.h"
#include "GHFileC.h"

GHFile* GHFOpenFileOpener::openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const
{
	const char* flags = 0;
	if (fileType == GHFile::FT_BINARY)
	{
		if (fileMode == GHFile::FM_READONLY)
		{
			flags = "rb";
		}
		else if (fileMode == GHFile::FM_READWRITE)
		{
			flags = "r+b";
		}
		else
		{
			// FM_WRITEONLY
			flags = "wb";
		}
	}
	else 
	{
		// FT_TEXT
		if (fileMode == GHFile::FM_READONLY)
		{
			flags = "rt";
		}
		else if (fileMode == GHFile::FM_READWRITE)
		{
			flags = "r+t";
		}
		else
		{
			// FM_WRITEONLY
			flags = "wt";
		}
	}

	if (!filename) {
		return 0;
	}

	FILE* file = ::fopen(filename, flags);
	if (file) {
		GHFileC* ret = new GHFileC(file, fileType, fileMode);
		return ret;
	}
	return 0;
}

