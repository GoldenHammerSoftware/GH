// Copyright 2010 Golden Hammer Software
#include "GHGLESRenderTargetFactory.h"
#include "GHGLESTextureMgr.h"
#include "GHPlatform/GHMutex.h"
#include "GHGLESRenderTarget.h"
#include "GHGLESTexture.h"
#include "GHTextureData.h"
#include <stdlib.h>
#include "GHGLESExtensions.h"

GHGLESRenderTargetFactory::GHGLESRenderTargetFactory(GHEventMgr& eventMgr, GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, GHMutex* mutex)
: mEventMgr(eventMgr)
, mTextureMgr(textureMgr)
, mStateCache(stateCache)
, mMutex(mutex)
{
	if (!GHGLESExtensions::checkAtLeastES3())
	{
		if (!GHGLESExtensions::checkExtension("OES_depth_texture"))
		{
			mAllowDepthTarget = false;
		}
	}
#ifdef GH_IOS
    // need to debug why GL_DEPTH_COMPONENT with glTexImage2D is failing.
    mAllowDepthTarget = false;
#endif
}

GHRenderTarget* GHGLESRenderTargetFactory::createRenderTarget(const GHRenderTarget::Config& config) const
{
	if (mMutex) mMutex->acquire();
	GHRenderTarget::Config modConfig = modifyTargetConfig(config);
	GHGLESTexture* tex = createTexture(modConfig);
	GHRenderTarget* ret = new GHGLESRenderTarget(tex, mEventMgr, modConfig);
	if (mMutex) mMutex->release();
	return ret;
}

void GHGLESRenderTargetFactory::resize(GHRenderTarget& target, const GHRenderTarget::Config& config) const
{
    GHGLESRenderTarget& glesTarget = reinterpret_cast<GHGLESRenderTarget&>(target);
    unsigned int oldWidth, oldHeight, oldDepth;
    glesTarget.getTexture()->getDimensions(oldWidth, oldHeight, oldDepth);
    if (oldWidth == config.mWidth && oldHeight == config.mHeight && oldDepth == 4)
    {
        return;
    }
    
    if (mMutex) mMutex->acquire();
	GHRenderTarget::Config modConfig = modifyTargetConfig(config);
	GHGLESTexture* glesTexture = (GHGLESTexture*)glesTarget.getTexture();
	GHGLESTexture* tex = createTexture(modConfig);
    glesTarget.setTexture(tex, modConfig);
    if (mMutex) mMutex->release();
}

GHGLESTexture* GHGLESRenderTargetFactory::createTexture(const GHRenderTarget::Config& config) const
{
	GHTextureData* texData = new GHTextureData();
	texData->mSrgb = false;

	if (config.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		// todo: check es3 or GL_OES_depth_texture
		// https://www.khronos.org/registry/OpenGL/extensions/OES/OES_depth_texture.txt
		texData->mChannelType = GHTextureChannelType::TC_USHORT;
		texData->mTextureFormat = GHTextureFormat::TF_DEPTH;
		texData->mDepth = 0;
	}
	else
	{
		texData->mChannelType = GHTextureChannelType::TC_UBYTE;
		texData->mTextureFormat = GHTextureFormat::TF_RGBA8;
		texData->mDepth = 4;
	}

	texData->mMipLevels.resize(1);
	texData->mMipLevels[0].mWidth = config.mWidth;
	texData->mMipLevels[0].mHeight = config.mHeight;

//#define WASTE_RT_MEMORY 1
#ifndef WASTE_RT_MEMORY
    // We don't need mData, so newing it here is a huge waste of memory.
    // data size is only needed for compressed types and we probably won't have a compressed target.
	texData->mMipLevels[0].mDataSize = 0;
	// It's ok to pass null to glTexImage2D.
	texData->mMipLevels[0].mData = NULL;
#else
    texData->mMipLevels[0].mDataSize = config.mWidth * config.mHeight * 4;
    texData->mMipLevels[0].mData = new int8_t[texData->mMipLevels[0].mDataSize];
#endif
    
	texData->mDataSource = (int8_t*)texData->mMipLevels[0].mData;

	bool mipmap = GHGLESRenderTarget::usesMipmaps(config);
	GHGLESTexture* tex = new GHGLESTexture(mTextureMgr, mStateCache, texData, mipmap, 0, 0);
    return tex;
}

GHRenderTarget::Config GHGLESRenderTargetFactory::modifyTargetConfig(const GHRenderTarget::Config& config) const
{
	GHRenderTarget::Config modConfig = config;
	if (!mAllowDepthTarget && config.mType == GHRenderTarget::RT_DEPTHONLY)
	{
		modConfig.mType = GHGLESRenderTarget::RT_DEFAULT;
	}

	// test hack: disable msaa
	//modConfig.mMsaa = false;
	//modConfig.mMipmap = false;
	return modConfig;
}
