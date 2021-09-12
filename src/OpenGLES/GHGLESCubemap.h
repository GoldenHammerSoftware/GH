#pragma once

#include "GHGLESTexture.h"
#include <vector>

// A GHGLESTexture which itself contains 6 GHGLESTexture objects representing cubemap faces
//  Potentially we could save GPU memory by having either multiple GHTextureDatas or adding support for
//  faces to GHTextureData instead of having 6 complete GHGLESTextures (or at least just have them not call createOGLTexture internally)
//	This is somewhat difficult to do while still leveraging the existing loading pipeline for various texture types because all loaders 
//  create full-fledged GHGLESTextures.
//  It is also not possible to get enough information to construct GHTextureData just from the GHTexture interface, although this might be amendable.
class GHGLESCubemap : public GHGLESTexture
{
public:
	GHGLESCubemap(GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache,
				  std::vector<GHGLESTexture*>&& faceTextures,
				  bool autoGenerateMips,
				  const GHTextureDataLoader* dataLoader, const char* sourceName);

	// not overriding bind: we just change mBindType to GL_TEXTURE_CUBE_MAP
	//virtual void bind(void);

	virtual bool lockSurface(void** ret, unsigned int& pitch);
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);

	virtual void deleteSourceData(void);

protected:
	~GHGLESCubemap(void);
	virtual void createOGLTexture(void);

private:
	bool subsequentDimensionsMatch(unsigned int firstWidth, unsigned int firstHeight, unsigned int firstDepth);

private:
	std::vector<GHGLESTexture*> mFaceTextures;
};
