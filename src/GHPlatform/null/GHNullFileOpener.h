// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHFileOpener.h"

class GHNullFileOpener : public GHFileOpener
{
protected:
    virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const { return 0; }
};
