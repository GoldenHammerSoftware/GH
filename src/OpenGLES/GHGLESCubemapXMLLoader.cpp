#include "GHGLESCubemapXMLLoader.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESTexture.h"
#include <vector>
#include "GHGLESCubemap.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHRenderProperties.h"
#include "GHGhcmXmlParser.h"

GHGLESCubemapXMLLoader::GHGLESCubemapXMLLoader(GHResourceFactory& resourceFactory, GHGLESTextureMgr& textureMgr, GHGLESStateCache& stateCache) 
	: mResourceFactory(resourceFactory)
	, mTextureMgr(textureMgr)
	, mStateCache(stateCache)
{

}


namespace {
	struct LoadedFaces
	{
		std::vector<GHGLESTexture*> mFaces;
		LoadedFaces(void) { mFaces.reserve(6); }
		~LoadedFaces(void)
		{
			for(GHGLESTexture* face : mFaces)
			{
				if (face) {
					face->release();
				}
			}
		}
	};
}


void* GHGLESCubemapXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    // Hack: force texture data to stick around after being uploaded so that it can be re-uploaded as a cubemap face.
    GHPropertyStacker keepTextureDataStacker(extraData, GHRenderProperties::GP_KEEPTEXTUREDATA, true);
    
    GHTexture* faceArray[6];
    bool parsed = GHGhcmXmlParser::parseGHCM(mResourceFactory, node, extraData, faceArray);
    if (!parsed)
    {
        GHDebugMessage::outputString("Failed to parse cubemap faces");
    }

	LoadedFaces faces;

	for (int i = 0; i < 6; ++i)
	{
		faces.mFaces.push_back((GHGLESTexture*)faceArray[i]);
	}

	//todo: support loading mipmapped cubemaps
	bool autoGenerateMips = true;

	GHGLESCubemap* ret = new GHGLESCubemap(mTextureMgr, mStateCache, std::move(faces.mFaces), autoGenerateMips, nullptr, nullptr);

	return ret;
}

