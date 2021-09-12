// Copyright 2010 Golden Hammer Software
#include "GHRenderTargetFactoryOGL.h"
#include "GHRenderTargetOGL.h"
#include "GHMutex.h"
#include "GHOGLInclude.h"
#include "GHTextureOGL.h"
#include <stdlib.h>

GHRenderTargetFactoryOGL::GHRenderTargetFactoryOGL(GHOGLTextureMgr& textureMgr, GHMutex* mutex)
: mTextureMgr(textureMgr)
, mMutex(mutex)
{
}

GHRenderTarget* GHRenderTargetFactoryOGL::createRenderTarget(unsigned int width, 
                                                             unsigned int height, bool mipmap) const
{
    // ignoring mipmap
    
	GLubyte* spriteData = (GLubyte *) malloc(width * height * 4);
	if (mMutex) mMutex->acquire();
	GHTextureOGL* tex = new GHTextureOGL(mTextureMgr, (unsigned char*)spriteData, 
                                         width, height, 4, false, false);
	GHRenderTarget* ret = new GHRenderTargetOGL(tex);
	if (mMutex) mMutex->release();
	return ret;
}
