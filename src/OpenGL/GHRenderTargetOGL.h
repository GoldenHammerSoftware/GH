// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderTarget.h"

class GHTextureOGL;

class GHRenderTargetOGL : public GHRenderTarget
{
public:
	GHRenderTargetOGL(GHTextureOGL* texture);
	virtual ~GHRenderTargetOGL(void);
	
	virtual void apply(void);
	virtual void remove(void);
	virtual GHTexture* getTexture(void);
    
private:
	void createOGLTarget(void);
	
protected:
	GHTextureOGL* mTexture;
	unsigned int mFBO;
    unsigned int mDepthBuffer;
    // we store out the viewport in apply to put it back in remove.
	int mReplacedViewportWidth;
	int mReplacedViewportHeight;
};
