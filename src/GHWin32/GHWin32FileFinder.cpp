// Copyright Golden Hammer Software
#include "GHWin32FileFinder.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/win32/GHLChar.h"

GHWin32FileFinder::GHWin32FileFinder(const GHFileOpener& fileOpener)
: mFileOpener(fileOpener)
{
}

bool GHWin32FileFinder::fileExists(const char* filename) const
{
	FILE* file = ::fopen(filename, "rb");
	if (file) {
		fclose(file);
		return true;
	}
	return false;
}

bool GHWin32FileFinder::createFilePath(const char* filename, wchar_t* ret, size_t retSize, bool lookInReadOnlyDir) const
{
	if (!filename)
	{
		GHDebugMessage::outputString("null filename passed to GHWin32FileFinder::createFilePath");
		return false;
	}
	// go through the resource directories and figure out where the file lives.
	char testPath[_MAX_PATH];
	bool foundFile = false;
	const std::vector<GHString>& rpaths = mFileOpener.getResourcePaths();
	for (auto i = rpaths.rbegin(); i != rpaths.rend(); ++i)
	{
		const GHString& path = *i;
		sprintf(testPath, "%s/%s", path.getChars(), filename);
		if (fileExists(testPath))
		{
			foundFile = true;
			break;
		}
	}
	if (!foundFile) 
	{
		if (fileExists(filename))
		{
			sprintf(testPath, "%s", filename);
			foundFile = true;
		}
	}
	if (!foundFile)
	{
		//GHDebugMessage::outputString("Could not find texture file %s", filename);
		return false;
	}

	GHLChar::convertToWide(testPath, ret, retSize);
	return true;
}
