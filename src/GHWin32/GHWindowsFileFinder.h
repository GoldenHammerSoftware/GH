#pragma once

// Interface for finding files in any of our data directories on windows.
// Interface exists for win32/winrt
class GHWindowsFileFinder
{
public:
	virtual ~GHWindowsFileFinder() = default;
	virtual bool createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const = 0;
};