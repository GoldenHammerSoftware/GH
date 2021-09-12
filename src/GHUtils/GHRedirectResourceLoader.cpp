// Copyright Golden Hammer Software
#include "GHRedirectResourceLoader.h"
#include "GHResourceFactory.h"
#include <assert.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHString/GHFilenameUtil.h"

GHRedirectResourceLoader::GHRedirectResourceLoader(GHResourceFactory& cache, const char* ext)
: mCache(cache)
{
	size_t len = strlen(ext);
	assert(len < GHRedirectResourceLoader::MAX_EXT_LEN);
	memset(mExt, 0, GHRedirectResourceLoader::MAX_EXT_LEN);
	if (len < GHRedirectResourceLoader::MAX_EXT_LEN)
	{
		strcpy(mExt, ext);
	}
}

GHResource* GHRedirectResourceLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHFilenameUtil::replaceExtension(filename, mExt, mScratchBuffer, GHRedirectResourceLoader::MAX_FILENAME_LEN + GHRedirectResourceLoader::MAX_EXT_LEN);
	return mCache.getUniqueResource(mScratchBuffer, extraData);
}

