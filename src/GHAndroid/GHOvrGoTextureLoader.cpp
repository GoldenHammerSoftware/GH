// Copyright Golden Hammer Software
#include "GHOvrGoTextureLoader.h"
#include "GHTextureFormat.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHTextureDataFactoryASTC.h"
#include "GHGLESTexture.h"
#include "GHTextureData.h"


//Todo: make GHOvrGoTextureLoader::loadMemory replace the version that is in GHAndroidBitmapLoader.
//	Register it for ".ovrtex"
//	Explicitly call GHTextureDataFactoryASTC (explicit code may be needed for the no header version with mipmaps).
//  and pass the result into a new GHGLESTexture
//	Ideally, no Avatar code should need to change (or texture code)
//  It may make sense to put any versions of astc loading needed for OVR in GHTextureDataFactoryASTC though.

GHOvrGoTextureLoader::GHOvrGoTextureLoader(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache)
	: mTextureMgr(textureMgr)
	, mStateCache(stateCache)
{

}

GHOvrGoTextureLoader::~GHOvrGoTextureLoader(void)
{

}

GHResource* GHOvrGoTextureLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	assert(false && "GHOvrGoTextureLoader::loadFile not implemented: we don't load .ovrtex from file");
	return 0;
}

GHResource* GHOvrGoTextureLoader::loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData)
{
	assert(extraData && "Error: GHOvrGoTextureLoader::loadMemory can't load raw memory without some metadata describing how to load it");
	if (!extraData) { return 0; }
	
	GHTextureFormat::Enum textureFormat = extraData->getProperty(GHRenderProperties::GP_TEXTUREFORMAT);
	uint32_t width = extraData->getProperty(GHRenderProperties::GP_WIDTH);
	uint32_t height = extraData->getProperty(GHRenderProperties::GP_HEIGHT);
	uint32_t numMips = extraData->getProperty(GHRenderProperties::GP_NUMMIPS);
	bool noHeader = extraData->getProperty(GHRenderProperties::GP_NOHEADER);
	int keepTexturedata = extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);

	if (textureFormat == GHTextureFormat::TF_ASTC_6x6)
	{
		GHTextureDataFactoryASTC textureDataFactory;
		GHTextureData* textureData = textureDataFactory.createFromMemoryWithMips(mem, memSize, textureFormat, width, height, numMips, !noHeader);
		if (keepTexturedata)
		{
			//"keepTextureData" means we're not allowed to delete it. 
			// The image data used for rendering purposes will come from mipLevels.
			// mDataSource is only a cache for deletion.
			textureData->mDataSource = 0;
		}
		return new GHGLESTexture(mTextureMgr, mStateCache, textureData, false, 0, 0);
	}
	assert(false && "GHAndroidBitmapLoader::loadMemory: texture format not yet supported");
	return 0;
}