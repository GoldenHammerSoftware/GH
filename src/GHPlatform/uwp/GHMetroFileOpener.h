// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHFileOpener.h"

class GHMetroFile;

class GHMetroFileOpener : public GHFileOpener
{
public:
	GHMetroFileOpener(const wchar_t* readSubPath, const wchar_t* writeSubPath);

	// take a filename and find out where it is expected to live.
	bool createFilePath(const char* filename, bool forWrite, wchar_t* ret, size_t retSize) const;

protected:
	virtual GHFile* openPlatformFile(const char* filename, GHFile::FileType fileType, GHFile::FileMode fileMode) const;

private:
	GHMetroFile* createFile(Platform::String^ path, const char* filename, GHFile::FileMode fileMode) const;

private:
	Platform::String^ mReadPath;
	Platform::String^ mWritePath;
};
