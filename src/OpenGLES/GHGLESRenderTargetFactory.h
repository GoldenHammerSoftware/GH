// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderTargetFactory.h"

class GHGLESTextureMgr;
class GHMutex;
class GHEventMgr;
class GHGLESTexture;
class GHGLESStateCache;

class GHGLESRenderTargetFactory : public GHRenderTargetFactory
{
public:
	GHGLESRenderTargetFactory(GHEventMgr& eventMgr, GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache, GHMutex* mutex);

	virtual GHRenderTarget* createRenderTarget(const GHRenderTarget::Config& config) const;
	virtual void resize(GHRenderTarget& target, const GHRenderTarget::Config& config) const;
    
private:
	GHRenderTarget::Config modifyTargetConfig(const GHRenderTarget::Config& config) const;
    GHGLESTexture* createTexture(const GHRenderTarget::Config& config) const;

protected:
    GHEventMgr& mEventMgr;
	GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;
	GHMutex* mMutex{ 0 };
	bool mAllowDepthTarget{ true };
};
