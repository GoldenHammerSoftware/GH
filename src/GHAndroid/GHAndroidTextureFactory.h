#pragma once

#include <jni.h>

class GHGLESTextureMgr;
class GHAndroidTexture;
class GHJNIMgr;
class GHGLESStateCache;

class GHAndroidTextureFactory 
{
public:
    GHAndroidTextureFactory(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache);
    
    GHAndroidTexture* createTexture(GHJNIMgr& jniMgr, jobject thiz, jobject bitmap,
                                    jint width, jint height, jint depth, 
                                    int textureId);
    
    void setIsGeneratingMips(bool isGenerating) { mIsGeneratingMips = isGenerating; }
    
private:
    GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;
    bool mIsGeneratingMips;
};
