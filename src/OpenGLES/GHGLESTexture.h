// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHTexture.h"
#include "GHTextureData.h"
#include "GHString/GHString.h"
#include "GHGLESInclude.h"

class GHGLESTextureMgr;
class GHTextureDataLoader;
class GHGLESStateCache;

class GHGLESTexture : public GHTexture
{
public:
	// Arguments:
	// textureMgr: used for registering for device lost and getting gl texture handles.
	// textureData: our source data, used for uploading to opengl.  we take ownership.
	// autoGenerateMips: if we are uncompressed then glGenerateMipmap if no mips in textureData
	// dataLoader/sourceName: if deleteSourceData is called we need these to recreate from disk.
	//  otherwise it's ok to pass in 0,0 for these.
	GHGLESTexture(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache,
		GHTextureData* textureData, bool autoGenerateMips,// bool doCreateOGLTexture,
		const GHTextureDataLoader* dataLoader, const char* sourceName);

    virtual void bind(void);

	virtual bool lockSurface(void** ret, unsigned int& pitch);
	virtual bool unlockSurface(void) { return true; }
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);
	
	unsigned int getTextureId(void) const { return mTextureId; }
	
	virtual void deleteSourceData(void);
    
    //Called by DeviceListener.
    // virtual because this class may be subclassed on certain platforms.
    // not part of GHTexture interface
    virtual void onDeviceReinit(void);

	bool isMipmap(void) const { return mMipmap; }

	// Called by Cubemap upload to upload individual faces -- not part of GHTexture interface
	virtual void uploadToOGL(GLint mipLevel, GLint target);
	
protected:
	~GHGLESTexture(void);
	virtual void createOGLTexture(void);

private:
	bool determineMipmapStatus(bool autoGenerateMips) const;

protected:
	GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;

	unsigned int mTextureId{ 0 };
	GHTextureData* mTextureData{ 0 };

	// Should we ask gl to generate mips for us if mTextureData does not have them?
	bool mAutoGenerateMipmaps{ false };
	// Do we have mips?  True if either mAutoGenerateMips or our texture data once had mips.
	bool mMipmap{ false };

	// Info needed to recreate the texture from disk without keeping mTextureData around.
	const GHTextureDataLoader* mDataLoader;
	GHString mSourceName;

	GLint mBindType{ GL_TEXTURE_2D }; //or GL_TEXTURE_CUBE_MAP
};
