// Copyright 2010 Golden Hammer Software
#include "GHTextureOGL.h"
#include "GHOGLTextureMgr.h"
#include "GHGLErrorReporter.h"

GHTextureOGL::GHTextureOGL(GHOGLTextureMgr& textureMgr, unsigned char* pImageBuffer, 
                           unsigned int bufferWidth, unsigned int bufferHeight, 
                           unsigned int bufferDepth, bool mipMap, bool compress)
:	mImageBuffer(pImageBuffer) 
,	mWidth(bufferWidth)
,	mHeight(bufferHeight)
,	mDepth(bufferDepth)
,	mTextureMgr(textureMgr)
,	mTextureId(textureMgr.getUnusedHandle())
,	mMipMap(mipMap)
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	GHGLErrorReporter::checkAndReportError("glBindTexture");
		
	int internalFormat = 4;
	if (compress)
	{
		internalFormat = GL_COMPRESSED_RGB_S3TC_DXT1_EXT;
		//internalFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
	}
    
	glTexImage2D(GL_TEXTURE_2D, 0, internalFormat,
				 bufferWidth, bufferHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, 
				 pImageBuffer);
	GHGLErrorReporter::checkAndReportError("glTexImage2D");
        
	if (mMipMap) {
		glGenerateMipmap(GL_TEXTURE_2D);
		GHGLErrorReporter::checkAndReportError("glGenerateMipmap");
	}
}

GHTextureOGL::~GHTextureOGL(void)
{
	mTextureMgr.returnHandleToPool(mTextureId);
	if (mImageBuffer) {
		delete mImageBuffer;
	}
}

void GHTextureOGL::bind(void)
{
	glBindTexture(GL_TEXTURE_2D, mTextureId);
    
	if (mMipMap) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
	}
}

bool GHTextureOGL::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth) 
{ 
	width = mWidth;
	height = mHeight;
	depth = mDepth/8;
	return true; 
}

void GHTextureOGL::deleteSourceData(void)
{
	delete mImageBuffer;
	mImageBuffer = 0;
}
