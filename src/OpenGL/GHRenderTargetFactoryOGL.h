// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHRenderTargetFactory.h"

class GHOGLTextureMgr;
class GHMutex;

class GHRenderTargetFactoryOGL : public GHRenderTargetFactory
{
public:
	GHRenderTargetFactoryOGL(GHOGLTextureMgr& textureMgr, GHMutex* mutex);
	virtual GHRenderTarget* createRenderTarget(unsigned int width, unsigned int height, bool mipmap = false) const;
	virtual void resize(GHRenderTarget& target, unsigned int width, unsigned int height) const {} // todo

protected:
	GHOGLTextureMgr& mTextureMgr;
	GHMutex* mMutex;
};
