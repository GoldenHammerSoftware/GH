// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTexture.h"

class GHOGLTextureMgr;

class GHTextureOGL : public GHTexture
{
public:
	GHTextureOGL(GHOGLTextureMgr& textureMgr, unsigned char* pImageBuffer, 
                 unsigned int bufferWidth, unsigned int bufferHeight, unsigned int bufferDepth,
                 bool mipmap, bool compress);
    
	virtual bool lockSurface(void** ret) { *ret = mImageBuffer; return true; }
	virtual bool unlockSurface(void) { return true; }
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);
	
	virtual void bind(void);
	virtual void deleteSourceData(void);
	unsigned int getTextureId(void) const { return mTextureId; }

protected:
	~GHTextureOGL(void);
    
private:
	unsigned char* mImageBuffer; 
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mDepth;	
	GHOGLTextureMgr& mTextureMgr;
	unsigned int mTextureId;
	bool mMipMap;
};
