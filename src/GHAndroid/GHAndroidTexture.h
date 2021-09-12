// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHGLESTexture.h"
#include <jni.h>
class GHJNIMgr;

class GHAndroidTexture : public GHGLESTexture
{
public:
	GHAndroidTexture(GHJNIMgr& jniMgr, jobject jobj, const char* srcFileName, jobject bitmap,
                     GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, int javaTextureId, bool mipmap);
	~GHAndroidTexture(void);

    virtual void onDeviceReinit(void);

	virtual bool lockSurface(void** ret, unsigned int& pitch);
	virtual bool unlockSurface(void);
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);

	//from GHGLESTExture
	virtual void uploadToOGL(GLint mipLevel, GLint target);
    
private:
	void debugPrintAndroidBitmapErrorCode(const char* method, int errorCode) const;

private:
	GHJNIMgr& mJNIMgr;
	jobject mJObject; //the GHEngineInterface java obj
	jobject mBitmap; // the android.graphics.Bitmap
	jmethodID mDestructorMethodID;
	jmethodID mApplyMethodID;
	jmethodID mOGLUploadMethodID;
	int mJavaTextureId;
};