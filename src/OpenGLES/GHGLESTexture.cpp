// Copyright 2010 Golden Hammer Software
#include "GHGLESTexture.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESErrorReporter.h"
#include "GHGLESInclude.h"
#include "GHGLESTextureMgr.h"
#include "GHTextureTypeUtil.h"
#include "GHTextureDataLoader.h"
#include "GHGLESStateCache.h"

GHGLESTexture::GHGLESTexture(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache,
	GHTextureData* textureData, bool autoGenerateMips,
	const GHTextureDataLoader* dataLoader, const char* sourceName)
:	mTextureData(textureData) 
,	mStateCache(stateCache)
,	mAutoGenerateMipmaps(autoGenerateMips)
,	mTextureMgr(textureMgr)
,	mDataLoader(dataLoader)
,	mSourceName(sourceName, GHString::CHT_COPY)
{
	mMipmap = determineMipmapStatus(autoGenerateMips);
	createOGLTexture();
    mTextureMgr.registerTexture(this);
}

GHGLESTexture::~GHGLESTexture(void)
{
	GHGLESErrorReporter::checkAndReportError("pre ~GHGLESTexture");

    mTextureMgr.unregisterTexture(this);

	// Is this really necessary?
	// Bind a tiny garbage texture in place of our real texture.
	glBindTexture(GL_TEXTURE_2D, mTextureId);
	GHGLESErrorReporter::checkAndReportError("~GHGLESTexture - bind");
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 4, 4, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
	GHGLESErrorReporter::checkAndReportError("~GHGLESTexture - glTexImage2D");

	mTextureMgr.returnHandleToPool(mTextureId);
	deleteSourceData();
}

void GHGLESTexture::deleteSourceData(void)
{
	if (mTextureData)
	{
		delete mTextureData;
		mTextureData = 0;
	}
}

void GHGLESTexture::bind(void)
{
	GHGLESErrorReporter::checkAndReportError("pre GHGLESTexture::bind");
	// Todo: support non-2d textures.
	glBindTexture(mBindType, mTextureId);
	GHGLESErrorReporter::checkAndReportError("GHGLESTexture::glBindTexture");
	
	mStateCache.setTextureMipmap(mMipmap, mBindType);
	
	GHGLESErrorReporter::checkAndReportError("bindTexture - glTexParameterf");
}

void GHGLESTexture::createOGLTexture(void)
{
	GHGLESErrorReporter::checkAndReportError("pre GHGLESTexture::createOGLTexture");

    mTextureId = mTextureMgr.getUnusedHandle();
    bind();

    if (mTextureData)
    {
		if (!mTextureData->mMipLevels.size())
		{
			return;
		}

		int numMipLevels = (int)mTextureData->mMipLevels.size();
		for (int mipLevel = 0; mipLevel < numMipLevels; ++mipLevel)
		{
			uploadToOGL(mipLevel, GL_TEXTURE_2D);
		}

		// I think glGenerateMipmap won't work with compressed formats.
		if (mAutoGenerateMipmaps && mTextureData->mMipLevels.size() < 2
			&& !GHTextureTypeUtil::isCompressedType(mTextureData->mTextureFormat))
		{
			glGenerateMipmap(GL_TEXTURE_2D);
			GHGLESErrorReporter::checkAndReportError("CreateOGLTexture glGenerateMipmap");
		}
    }
}

void GHGLESTexture::uploadToOGL(GLint mipLevel, GLint target)
{
	if (!GHTextureTypeUtil::isCompressedType(mTextureData->mTextureFormat))
	{
		// todo: support srgb
		// todo: support times where internalformat and format are different.
		GLint format = GHTextureTypeUtil::getGLFormat(mTextureData->mTextureFormat);
		GLint internalFormat = format;
        
		GLenum channelType = GHTextureTypeUtil::getGLChannelType(mTextureData->mChannelType);
		GLint border = 0;

        // test hack for trying to debug depth on ios
        /*
        if (mTextureData->mTextureFormat == GHTextureFormat::TF_DEPTH)
        {
            format = GL_DEPTH_COMPONENT;
            internalFormat = GL_DEPTH_COMPONENT;
            channelType = GL_UNSIGNED_SHORT;
            
            //format = GL_DEPTH_STENCIL;
            //internalFormat = GL_DEPTH_STENCIL;
            //channelType = GL_UNSIGNED_INT_24_8;
        }
        */

		glTexImage2D(target, mipLevel, internalFormat,
					mTextureData->mMipLevels[mipLevel].mWidth, mTextureData->mMipLevels[mipLevel].mHeight, border,
					format, channelType, mTextureData->mMipLevels[mipLevel].mData);
		GHGLESErrorReporter::checkAndReportError("CreateOGLTexture glTexImage2D");
	}
	else
	{
		GLint format = GHTextureTypeUtil::getGLFormat(mTextureData->mTextureFormat);
		GLint border = 0; // must be 0.

		glCompressedTexImage2D(target, mipLevel, format,
							   mTextureData->mMipLevels[mipLevel].mWidth, mTextureData->mMipLevels[mipLevel].mHeight, border,
							   mTextureData->mMipLevels[mipLevel].mDataSize, mTextureData->mMipLevels[mipLevel].mData);
		GHGLESErrorReporter::checkAndReportError("glCompressedTexImage2D");
	}
}

void GHGLESTexture::onDeviceReinit(void)
{
	// Our old handle is already deleted so we don't need to return it to pool.
	// Doing so would potentially delete an unknown texture.

	if (mTextureData || !mDataLoader)
	{
		// If we still have our texture data or have no way to recreate it just call createOGLTexture.
		// Android java bitmap will call this function and expect us to grab a new texture handle.
		createOGLTexture();
	}
	else
	{
		// reload from disk if possible.
		mTextureData = mDataLoader->load(mSourceName.getChars());
		createOGLTexture();
		deleteSourceData();
	}
}

bool GHGLESTexture::lockSurface(void** ret, unsigned int& pitch)
{ 
	if (!mTextureData || !mTextureData->mMipLevels.size())
	{
		*ret = 0;
		return false;
	}
	*ret = mTextureData->mMipLevels[0].mData;

	// it is possible that pitch is actually something different and this call is wrong?
	pitch = mTextureData->mMipLevels[0].mWidth * mTextureData->mDepth;

	return true; 
}

bool GHGLESTexture::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
	if (!mTextureData || !mTextureData->mMipLevels.size())
	{
		width = 0;
		height = 0;
		depth = 0;
		return false;
	}

	width = mTextureData->mMipLevels[0].mWidth;
	height = mTextureData->mMipLevels[0].mHeight;
	depth = mTextureData->mDepth;
	return true;
}

bool GHGLESTexture::determineMipmapStatus(bool autoGenerateMips) const
{
	bool mipmap = false;
	if (mTextureData)
	{
		mipmap = mTextureData->mMipLevels.size() > 1;
		if (!mipmap && autoGenerateMips)
		{
			mipmap = !GHTextureTypeUtil::isCompressedType(mTextureData->mTextureFormat);
		}
	}
	else
	{
		mipmap = mAutoGenerateMipmaps;
	}
	return mipmap;
}
