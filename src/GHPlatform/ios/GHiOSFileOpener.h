// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHFileOpener.h"

class GHiOSFileOpener : public GHFileOpener
{
protected:
    virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;    

private:
	FILE* openBundleFile(const char* filename, const char* flags) const;
	FILE* openDocumentsFile(const char* filename, const char* flags) const;
};
