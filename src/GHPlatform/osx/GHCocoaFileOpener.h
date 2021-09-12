// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHFileOpener.h"

// interface for opening a file.
class GHCocoaFileOpener : public GHFileOpener
{
public:
    GHCocoaFileOpener(void);
    
protected:
    virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;
    
private:
    FILE* openBundleFile(const char* filename, const char* flags) const;
    FILE* openDocumentsFile(const char* filename, const char* flags) const;
    NSString* getDocumentsPath(void) const;
};
