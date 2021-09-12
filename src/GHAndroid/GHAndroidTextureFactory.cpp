#include "GHAndroidTextureFactory.h"
#include "GHAndroidTexture.h"

GHAndroidTextureFactory::GHAndroidTextureFactory(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache)
: mTextureMgr(textureMgr)
, mStateCache(stateCache)
, mIsGeneratingMips(false)
{
    
}

GHAndroidTexture* GHAndroidTextureFactory::createTexture(GHJNIMgr& jniMgr, jobject thiz, jobject bitmap,
                                                    jint width, jint height, jint depth, 
                                                    int textureId)
{
    const char* srcFilename = "filename storage not implemented";
    return new GHAndroidTexture(jniMgr, thiz, srcFilename, bitmap, mTextureMgr, mStateCache, textureId, mIsGeneratingMips);
}