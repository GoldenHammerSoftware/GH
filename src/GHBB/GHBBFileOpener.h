// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHFileOpener.h"

class GHBBFileOpener : public GHFileOpener
{

protected:
	virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;
};
