// Copyright Golden Hammer Software
#pragma once

// interface for finding out where a texture lives so we can pass it to the os texture load functions.
class GHTextureFinderDX11
{
public:
	virtual ~GHTextureFinderDX11(void) {}

	// return false if could not create a valid path.
	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const = 0;
};
