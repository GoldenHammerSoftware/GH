// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include "GHFile.h"
// TODO: remove ghstring
#include "GHString/GHString.h"

// interface for opening a file.
class GHFileOpener
{
public:
    const static int MAX_PATH_LEN = 1024;

public:
    virtual ~GHFileOpener(void) {}
    
    // If outFoundPath is non-null we assume it is an array of MAX_PATH_LEN and will fill in the location of the found file.
    GHFile* openFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode, char* outFoundPath=0) const;

    void addResourcePath(const char* path);
    const std::vector<GHString>& getResourcePaths(void) const { return mResourcePaths; }
    
protected:
    virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const = 0;
    
private:
    // ordered list of directories to look in.
	// most recently added is the first path examined.
	std::vector<GHString> mResourcePaths;
};
