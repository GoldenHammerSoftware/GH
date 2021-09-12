// Copyright 2010 Golden Hammer Software
#include "GHBBRenderServices.h"
#include "GHBBGLESContext.h"
#include "GHBBJPGLoader.h"
#include "GHBBPNGLoader.h"
#include "GHSystemServices.h"
#include "GHAppShard.h"

GHBBRenderServices::GHBBRenderServices(GHSystemServices& systemServices, const GHStringHashTable& hashTable,
		screen_context_t& ctx)
: GHGLESRenderServices(systemServices, mMutex, hashTable)
, mContext(0)
{
	setGraphicsQuality(2);

	mContext = new GHBBGLESContext(ctx);

	int screenWidth, screenHeight;
	mContext->getScreenSize(screenWidth, screenHeight);
    mScreenInfo.setSize(GHPoint2i(screenWidth, screenHeight));
	mScreenInfo.setPixelSizeFactor(1/800.0f, 0.0f);

	init(*mContext);
}

GHBBRenderServices::~GHBBRenderServices(void)
{
	delete mContext;
}

void GHBBRenderServices::initAppShard(GHAppShard& appShard)
{
	GHGLESRenderServices::initAppShard(appShard);

	GHBBJPGLoader* jpgLoader = new GHBBJPGLoader(*mSystemServices.mFileOpener, appShard.mResourceFactory,
			*mTextureMgr);
    appShard.mResourceFactory.addLoader(jpgLoader, 2, ".jpg", ".pvr4");

	GHBBPNGLoader* pngLoader = new GHBBPNGLoader(*mSystemServices.mFileOpener, *mTextureMgr);
    appShard.mResourceFactory.addLoader(pngLoader, 1, ".png");
}
