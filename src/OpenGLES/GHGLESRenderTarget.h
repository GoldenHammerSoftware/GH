// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderTarget.h"
#include "GHRenderDeviceReinitListener.h"

class GHGLESTexture;
class GHRenderTargetMgr;
class GHEventMgr;

class GHGLESRenderTarget : public GHRenderTarget
{
public:
	GHGLESRenderTarget(GHGLESTexture* texture, GHEventMgr& eventMgr, const GHRenderTarget::Config& config);
	virtual ~GHGLESRenderTarget(void);
	
	virtual void apply(void);
	virtual void remove(void);
	virtual GHTexture* getTexture(void);
	    
    void setTexture(GHGLESTexture* texture, const GHRenderTarget::Config& config);
	
	// queries to see whether a config allows mipmaps and msaa
	static bool usesMipmaps(const GHRenderTarget::Config& config);
	static bool usesMsaa(const GHRenderTarget::Config& config);

private:
	void createOGLTarget(void);
	void onDeviceReinit(void);
	bool createFBO(unsigned int& fbo, bool msaaTarget);
	void resolveMsaa(void);

	// here we assume mTexture is the color buffer and we make a depth buffer.
	void initColorAndDepthFBO(unsigned int fbo, bool msaaTarget);
	// here we assume mTexture is the depth buffer and we don't make a color buffer.
	void initDepthOnlyFBO(unsigned int fbo);

protected:
	GHGLESTexture* mTexture{ 0 };
	unsigned int mFBO{ 0 };
	unsigned int mResolveFBO{ 0 }; // resolve target for msaa
	unsigned int mDepth{ 0 };
	int mReplacedViewportWidth{ 0 };
	int mReplacedViewportHeight{ 0 };
	GHRenderTarget::Config mConfig;

	GHRenderDeviceReinitListener<GHGLESRenderTarget> mDeviceListener;
	friend class GHRenderDeviceReinitListener<GHGLESRenderTarget>;
};
