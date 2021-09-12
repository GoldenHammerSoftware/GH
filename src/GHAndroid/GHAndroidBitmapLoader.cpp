// Copyright Golden Hammer Software
#include "GHAndroidBitmapLoader.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHTexture.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHAndroidTexture.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHRenderProperties.h"
#include "GHPlatform/android/GHJNIMgr.h"

GHTexture* sLastLoadedTexture = 0;
GHAndroidTextureFactory* sTextureFactory = 0;
GHJNIMgr* sJNIMgr;

extern "C"
__attribute__((visibility("default")))
void
Java_goldenhammer_ghbase_GHEngineInterface_handleTextureLoadConfirmed(JNIEnv* env, jobject thiz, jobject bitmap,
																	  jint width, jint height, jint depth,
																	  int textureId)
{
	sLastLoadedTexture = sTextureFactory->createTexture(*sJNIMgr, thiz, bitmap, width, height, depth, textureId);
}

GHAndroidBitmapLoader::GHAndroidBitmapLoader(GHJNIMgr& jniMgr, jobject jobj, const GHFileOpener& fileOpener,
	GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache)
	: mJNIMgr(jniMgr)
	, mJObject(jobj)
	, mMethodID(0)
	, mFileOpener(fileOpener)
	, mTextureFactory(textureMgr, stateCache)
{
	sJNIMgr = &jniMgr;

	jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJObject);
	if (cls == 0) {
		GHDebugMessage::outputString("GHAndroidBitmapLoader can't find class where loadJavaBitmap method should exist\n.");
	}
	mMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "loadJavaBitmap", "(Ljava/lang/String;IZ)V");
	if (mMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidBitmapLoader can't find loadJavaBitmap method in Java.\n");
	}
}

GHAndroidBitmapLoader::~GHAndroidBitmapLoader(void)
{
	for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
	{
		delete *iter;
	}
	mOverrideLoaders.clear();
}

GHResource* GHAndroidBitmapLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	// first see if any of our overrides would rather load the file.
	for (auto iter = mOverrideLoaders.begin(); iter != mOverrideLoaders.end(); ++iter)
	{
		GHResourceLoader* currLoader = *iter;
		GHResource* testRet = currLoader->loadFile(filename, extraData);
		if (testRet)
		{
			return testRet;
		}
	}

	bool generateMips = extraData ? !(bool)extraData->getProperty(GHRenderProperties::DONTUSEMIPMAPS) : true;

	bool keepTextureData = extraData ? (bool)extraData->getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA) : false;

	jstring javaFilename = mJNIMgr.getJNIEnv().NewStringUTF(filename);
	if (javaFilename == 0) {
		GHDebugMessage::outputString("GHAndroidBitmapLoader java won't give us a string. OOM?");
	}

	mTextureFactory.setIsGeneratingMips(generateMips);
	sLastLoadedTexture = 0;
	sTextureFactory = &mTextureFactory;

	int wantMip = generateMips ? 1 : 0;
	mJNIMgr.getJNIEnv().CallVoidMethod(mJObject, mMethodID, javaFilename, (jint)wantMip, (jboolean)keepTextureData);
	mJNIMgr.getJNIEnv().DeleteLocalRef(javaFilename);

	if (!sLastLoadedTexture) {
		GHDebugMessage::outputString("Failed to load texture %s", filename);
	}
	return sLastLoadedTexture;
}

void GHAndroidBitmapLoader::addOverrideLoader(GHResourceLoader* loaderToOwn)
{
	mOverrideLoaders.push_back(loaderToOwn);
}
