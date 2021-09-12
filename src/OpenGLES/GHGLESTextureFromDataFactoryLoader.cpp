#include "GHGLESTextureFromDataFactoryLoader.h"
#include "GHTextureDataFactory.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHGLESTexture.h"
#include <assert.h>
#include "GHPlatform/GHDebugMessage.h"

GHGLESTextureFromDataFactoryLoader::GHGLESTextureFromDataFactoryLoader(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, const GHFileOpener& fileOpener, GHTextureDataFactory* dataFactory)
	: mDataLoader(fileOpener, *dataFactory)
	, mDataFactory(dataFactory)
	, mTextureMgr(textureMgr)
	, mStateCache(stateCache)
{
	assert(dataFactory);
}

GHGLESTextureFromDataFactoryLoader::~GHGLESTextureFromDataFactoryLoader(void)
{
	delete mDataFactory;
}

GHResource* GHGLESTextureFromDataFactoryLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHTextureData* texData = mDataLoader.load(filename);
	if (!texData)
	{
		//GHDebugMessage::outputString("Failed to load tex data for %s", filename);
		return 0;
	}
	return new GHGLESTexture(mTextureMgr, mStateCache, texData, true, &mDataLoader, filename);
}

