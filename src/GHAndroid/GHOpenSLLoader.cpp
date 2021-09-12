// Copyright 2010 Golden Hammer Software
#include "GHOpenSLLoader.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHOpenSLHandle.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>
#include <android/asset_manager.h>
#include <android/asset_manager_jni.h>

GHOpenSLLoader::GHOpenSLLoader(GHJNIMgr& jniMgr, jobject jAssetMgr)
: mJNIMgr(jniMgr)
, mJAssetMgr(jAssetMgr)
{
}

GHResource* GHOpenSLLoader::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	JNIEnv& env = mJNIMgr.getJNIEnv();

	// use asset manager to open asset by filename
	GHDebugMessage::outputString("GHOpenSLLoader grabbing AAssetManager");
	AAssetManager* mgr = AAssetManager_fromJava(&env, mJAssetMgr);
	if (mgr == 0)
	{
		GHDebugMessage::outputString("GHOpenSLLoader failed to grab AAssetManager");
		return 0;
	}

	/*
	// convert Java string to UTF-8
	const char *utf8 = env.GetStringUTFChars(filename, NULL);
	if (utf8 == 0)
	{
		GHDebugMessage::outputString("GHOpenSLLoader failed to create utf8 string");
		return 0;
	}

	AAsset* asset = AAssetManager_open(mgr, utf8, AASSET_MODE_UNKNOWN);

	// release the Java string and UTF-8
	env.ReleaseStringUTFChars(env, filename, utf8);
	*/
	GHDebugMessage::outputString("GHOpenSLLoader opening AAsset");
	AAsset* asset = AAssetManager_open(mgr, filename, AASSET_MODE_UNKNOWN);

	// the asset might not be found
	if (asset == 0) {
		GHDebugMessage::outputString("Failed to find openSL asset %s", filename);
		return 0;
	}

	GHOpenSLHandleFDArgs* fdArgs = new GHOpenSLHandleFDArgs();

	// open asset as file descriptor
	GHDebugMessage::outputString("GHOpenSLLoader AAsset_openFileDescriptor");
	fdArgs->fd = AAsset_openFileDescriptor(asset, &fdArgs->start, &fdArgs->length);
	AAsset_close(asset);
	if (fdArgs->fd < 0)
	{
		GHDebugMessage::outputString("OpenSL failed to get file descriptor for %s", filename);
		delete fdArgs;
		return 0;
	}

	// configure audio source
	fdArgs->loc_fd = { SL_DATALOCATOR_ANDROIDFD, fdArgs->fd, fdArgs->start, fdArgs->length };
	fdArgs->format_mime = { SL_DATAFORMAT_MIME, NULL, SL_CONTAINERTYPE_UNSPECIFIED };
	fdArgs->audioSrc = { &fdArgs->loc_fd, &fdArgs->format_mime };

	GHDebugMessage::outputString("creating GHOpenSLHandle");
	return new GHOpenSLHandle(fdArgs);
}
