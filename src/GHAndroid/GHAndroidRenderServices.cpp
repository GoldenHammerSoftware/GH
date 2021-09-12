// Copyright 2010 Golden Hammer Software
#include "GHAndroidRenderServices.h"
#include "GHSystemServices.h"
#include "GHAppShard.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHAndroidGLESContext.h"
#include "GHAndroidBitmapLoader.h"
#include "GHUtils/GHRedirectResourceLoader.h"
#include "GHJNIMgr.h"

GHAndroidRenderServices::GHAndroidRenderServices(int screenWidth, int screenHeight,
	GHSystemServices& systemServices,
	const GHStringIdFactory& hashTable,
	GHJNIMgr& jniMgr,
	jobject jobj,
	const GHFileOpener& fileOpener)
: GHGLESRenderServices(systemServices, mMutex, hashTable)
, mJNIMgr(jniMgr)
, mJObj(jobj)
, mFileOpener(fileOpener)
{
	// pick an initial graphics quality based on some sort of hardware specs.
	int initialGraphicsQuality = 2;
	GHDebugMessage::outputString("Attempting to detect os version");
	jclass versionClass = mJNIMgr.getJNIEnv().FindClass( "android/os/Build$VERSION");
	if (versionClass != NULL)
	{
		jfieldID sdkIntFieldID = NULL;
		sdkIntFieldID = mJNIMgr.getJNIEnv().GetStaticFieldID(versionClass, "SDK_INT", "I");
		if (sdkIntFieldID != NULL)
		{
			jint sdkInt = mJNIMgr.getJNIEnv().GetStaticIntField(versionClass, sdkIntFieldID);
			GHDebugMessage::outputString("Detected android version %d", sdkInt);
			if (sdkInt < 24)
			{
				// old shitty phone.
				initialGraphicsQuality = 0;
			}
		}
	}

	setGraphicsQuality(initialGraphicsQuality);
    
    mContext = new GHAndroidGLESContext(systemServices.getEventMgr(), mScreenInfo, *mReinitializer);
    
    init(*mContext);
    
    mScreenInfo.setSize(GHPoint2i(screenWidth, screenHeight));
	mScreenInfo.setPixelSizeFactor(1/800.0f, 0.0f);
	mScreenInfo.setMinimumPixelHeight(600);
}

GHAndroidRenderServices::~GHAndroidRenderServices(void)
{
    delete mContext;
}

void GHAndroidRenderServices::initAppShard(GHAppShard& appShard)
{
	GHGLESRenderServices::initAppShard(appShard);
    
    GHAndroidBitmapLoader* bitmapLoader = new GHAndroidBitmapLoader(mJNIMgr, mJObj, mFileOpener, *mTextureMgr, mStateCache);
    // these are the formats claimed to be supported by the java/android class BitmapFactory
    appShard.mResourceFactory.addLoader(bitmapLoader, 4, ".png", ".jpg", ".gif", ".bmp");

	// ktx isn't really a hardware format but rather a wrapper for hardware formats.
	// at the moment we are only using it for etc2, so check etc2 support.
	if (checkETC2Support())
	{
		GHRedirectResourceLoader* ktxRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".ktx");
		bitmapLoader->addOverrideLoader(ktxRedirect);
	}

	if (checkASTCSupport())
	{
		GHRedirectResourceLoader* astcRedirect = new GHRedirectResourceLoader(appShard.mResourceFactory, ".astc");
		bitmapLoader->addOverrideLoader(astcRedirect);
	}
}
