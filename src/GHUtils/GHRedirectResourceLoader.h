// Copyright Golden Hammer Software
#pragma once

#include "GHResourceLoader.h"
#include "GHString/GHString.h"

class GHResourceFactory;

// Pops off the extension of any file passed in, 
//  and instead loads a file with a different extension.
class GHRedirectResourceLoader : public GHResourceLoader
{
public:
	GHRedirectResourceLoader(GHResourceFactory& cache, const char* ext);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);

private:
	GHResourceFactory& mCache;

	// Keeping around a scratch buffer as a memory optimization.
	// Otherwise we'll new/delete memory on every loadFile call.
	static const size_t MAX_EXT_LEN = 16;
	static const size_t MAX_FILENAME_LEN = 1024;
	char mExt[MAX_EXT_LEN];
	char mScratchBuffer[MAX_FILENAME_LEN + MAX_EXT_LEN + 1];
};
