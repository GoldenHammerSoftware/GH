// Copyright Golden Hammer Software
#include "GHFileOpener.h"

//#define GENERATE_DATA_COPY
#ifdef GENERATE_DATA_COPY
#include "GHPlatform/GHDebugMessage.h"
static void outputDataCopy(const char* filename, const char* filepath)
{
	const char* fileAppend = "";
	if (strstr(filename, ".xml") ||
		strstr(filename, ".gham"))
	{
		fileAppend = ".mp3";
	}
	GHDebugMessage::outputString("copy ..\\%s\\%s ..\\..\\..\\AndroidStudio\\GHBowling\\GHBowlingVR\\src\\main\\assets\\%s%s", filepath, filename, filename, fileAppend);
}
#endif

GHFile* GHFileOpener::openFile(const char* filename, GHFile::FileType fileType, 
                               GHFile::FileMode fileMode, char* outFoundPath) const
{
    GHFile* ret = 0;
    char nameBuff[MAX_PATH_LEN];

    std::vector<GHString>::const_reverse_iterator pathIter;
    for (pathIter = mResourcePaths.rbegin(); pathIter != mResourcePaths.rend(); ++pathIter)
    {
		::snprintf(nameBuff, MAX_PATH_LEN, "%s%s", pathIter->getChars(), filename);
        ret = openPlatformFile(nameBuff, fileType, fileMode);
        if (ret) {
#ifdef GENERATE_DATA_COPY
			outputDataCopy(filename, pathIter->getChars());
#endif
			if (outFoundPath) ::snprintf(outFoundPath, MAX_PATH_LEN, "%s", nameBuff);
			return ret;
        }
    }
    
    // failed to find it in our extended paths, so look in root path.
    // we were having an issue of our config files being saved in random read-only directories,
    //  and to combat that we were looking in root first, but that prevents loading the Tablet- versions.
    ret = openPlatformFile(filename, fileType, fileMode);
    if (ret) {
#ifdef GENERATE_DATA_COPY
        outputDataCopy(filename, "");
#endif
        if (outFoundPath) ::snprintf(outFoundPath, MAX_PATH_LEN, "%s", filename);
        return ret;
    }

    return ret;
}

void GHFileOpener::addResourcePath(const char* path)
{
    mResourcePaths.push_back(path);
}
