#pragma once
#pragma once

#include "GHXMLObjLoader.h"

class GHResourceFactory;
class GHGLESTextureMgr;
class GHGLESStateCache;

// Wrapper loader for combining 6 GHGLESTextures into one representing a cubemap.
// Supports any file format that we already support.
// Returns a GHTexture (GHGLESTExture)
//  Possible todo: promote as much of this as possible to platform-indepent, move to GHRender, add necessary cubemap specification interface to GHTexture 
// Example xml:
//<ghcm
//	posx="posx.png"
//  negx="negx.png"
//  posy="posy.png"
//  negy="negy.png"
//  posz="posz.png"
//  negz="negz.png">
//</ghcm>

class GHGLESCubemapXMLLoader : public GHXMLObjLoader
{
public:
	GHGLESCubemapXMLLoader(GHResourceFactory& resourceFactory, GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHResourceFactory& mResourceFactory;
	GHGLESTextureMgr& mTextureMgr;
	GHGLESStateCache& mStateCache;
};
