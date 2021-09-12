// Copyright 2010 Golden Hammer Software
#include "GHAndroidTexture.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/android/GHJNIMgr.h"
#include <android/bitmap.h>

GHAndroidTexture::GHAndroidTexture(GHJNIMgr& jniMgr, jobject jobj, const char* srcFileName, jobject bitmap,
								   GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, int javaTextureId, bool mipmap)
: GHGLESTexture(textureMgr, stateCache, 0, mipmap, 0, 0)
, mJavaTextureId(javaTextureId)
, mJNIMgr(jniMgr)
, mDestructorMethodID(0)
, mApplyMethodID(0)
{
    mJObject = mJNIMgr.getJNIEnv().NewGlobalRef(jobj);
	jclass cls = mJNIMgr.getJNIEnv().GetObjectClass(mJObject);
	if(cls == 0) {
		GHDebugMessage::outputString("GHAndroidTexture can't find class where methods should exist\n.");
	}

	mDestructorMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "freeBitmap", "(I)V");
	if(mDestructorMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidTexture can't find freeBitmap method in Java.\n");
	}
	
	mApplyMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "applyBitmap", "(IIZ)Landroid/graphics/Bitmap;");
	if(mApplyMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidTexture can't find applyBitmap method in Java.\n");
	}

	mOGLUploadMethodID = mJNIMgr.getJNIEnv().GetMethodID(cls, "uploadBitmapToOGL", "(Landroid/graphics/Bitmap;II)V");
	if (mOGLUploadMethodID == 0) {
		GHDebugMessage::outputString("GHAndroidTexture can't find uploadToOGL method in Java.\n");
	}

	mBitmap = mJNIMgr.getJNIEnv().NewGlobalRef(bitmap);
}

GHAndroidTexture::~GHAndroidTexture(void)
{
	// tell java to release the bitmap memory.
	mJNIMgr.getJNIEnv().DeleteGlobalRef(mBitmap);
	mJNIMgr.getJNIEnv().CallVoidMethod(mJObject, mDestructorMethodID, (jint)mJavaTextureId);
	mJNIMgr.getJNIEnv().DeleteGlobalRef(mJObject);
}

void GHAndroidTexture::onDeviceReinit(void)
{
	jboolean keepTextureData = mBitmap != 0;
	mJNIMgr.getJNIEnv().DeleteGlobalRef(mBitmap);
    GHGLESTexture::onDeviceReinit();
	int wantMip = 1;
	if (!mMipmap) wantMip = 0;
	jobject bitmap = mJNIMgr.getJNIEnv().CallObjectMethod(mJObject, mApplyMethodID, (jint)mJavaTextureId, (jint)wantMip, keepTextureData);
	mBitmap = mJNIMgr.getJNIEnv().NewGlobalRef(bitmap);
	mJNIMgr.getJNIEnv().DeleteLocalRef(bitmap);
}

void GHAndroidTexture::debugPrintAndroidBitmapErrorCode(const char* method, int errorCode) const
{
	switch (errorCode)
	{
	case ANDROID_BITMAP_RESULT_BAD_PARAMETER:
		GHDebugMessage::outputString("Error in %s: ANDROID_BITMAP_RESULT_BAD_PARAMETER (JNIEnv or bitmap jobject)", method);
		break;
	case ANDROID_BITMAP_RESULT_JNI_EXCEPTION:
		GHDebugMessage::outputString("Error in %s: ANDROID_BITMAP_RESULT_JNI_EXCEPTION", method);
		break;
	case ANDROID_BITMAP_RESULT_ALLOCATION_FAILED:
		GHDebugMessage::outputString("Error in %s: ANDROID_BITMAP_RESULT_ALLOCATION_FAILED", method);
		break;
	default:
		GHDebugMessage::outputString("Unknown error in %s", method);
		break;
	}
}

bool  GHAndroidTexture::lockSurface(void** ret, unsigned int& pitch)
{
	if (!mBitmap)
	{
		GHDebugMessage::outputString("Error calling GHAndroidTexture::lockSurface: the bitmap data was not stored with GHRenderProperties::GP_KEEPTEXTUREDATA");
		return false;
	}

	int successVal = AndroidBitmap_lockPixels(&mJNIMgr.getJNIEnv(), mBitmap, ret);
	if (successVal == ANDROID_BITMAP_RESULT_SUCCESS) 
	{
		AndroidBitmapInfo bmpInfo;
		successVal = AndroidBitmap_getInfo(&mJNIMgr.getJNIEnv(), mBitmap, &bmpInfo);
		if (successVal == ANDROID_BITMAP_RESULT_SUCCESS)
		{
			pitch = bmpInfo.stride;
		}
		else
		{
			pitch = 0;
		}
		return true;
	}
	
	debugPrintAndroidBitmapErrorCode("AndroidBitmap_lockPixels", successVal);
	return false;
}

bool  GHAndroidTexture::unlockSurface(void)
{
	if (!mBitmap)
	{
		GHDebugMessage::outputString("Error calling GHAndroidTexture::unlockSurface: the bitmap data was not stored with GHRenderProperties::GP_KEEPTEXTUREDATA");
		return false;
	}

	int successVal = AndroidBitmap_unlockPixels(&mJNIMgr.getJNIEnv(), mBitmap);
	if (successVal == ANDROID_BITMAP_RESULT_SUCCESS) {
		return true;
	}

	debugPrintAndroidBitmapErrorCode("AndroidBitmap_unlockPixels", successVal);
	return false;
}

bool  GHAndroidTexture::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
	if (!mBitmap)
	{
		GHDebugMessage::outputString("Error calling GHAndroidTexture::getDimensions: the bitmap data was not stored with GHRenderProperties::GP_KEEPTEXTUREDATA");
		return false;
	}

	AndroidBitmapInfo info;
	int successVal = AndroidBitmap_getInfo(&mJNIMgr.getJNIEnv(), mBitmap, &info);
	if (successVal != ANDROID_BITMAP_RESUT_SUCCESS)
	{
		debugPrintAndroidBitmapErrorCode("AndroidBitmap_getInfo", successVal);
		return false;
	}


	width = info.width;
	height = info.height;

	switch (info.format)
	{
	case ANDROID_BITMAP_FORMAT_A_8:
		depth = 1;
		break;
	case ANDROID_BITMAP_FORMAT_RGBA_4444:
		depth = 2;
		break;
	case ANDROID_BITMAP_FORMAT_RGBA_8888:
		depth = 4;
		break;
	case ANDROID_BITMAP_FORMAT_RGB_565:
		depth = 2;
		break;
	case ANDROID_BITMAP_FORMAT_NONE:
	default:
		depth = info.width > 0 ? info.stride / (info.width) : 0;
	}

	assert(info.stride == info.width*depth);

	return true;
}

void GHAndroidTexture::uploadToOGL(GLint mipLevel, GLint target)
{
	mJNIMgr.getJNIEnv().CallVoidMethod(mJObject, mOGLUploadMethodID, mBitmap, (jint)mipLevel, (jint)target);
}


