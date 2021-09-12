#include "GHBulletTextureLookupFrictionOverrideXMLLoader.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHResourceFactory.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHBulletTextureLookupFrictionOverride.h"
#include "GHXMLNode.h"
#include "GHRenderProperties.h"

GHBulletTextureLookupFrictionOverrideXMLLoader::GHBulletTextureLookupFrictionOverrideXMLLoader(GHResourceFactory& resourceCache)
	: mResourceCache(resourceCache)
{

}

void* GHBulletTextureLookupFrictionOverrideXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const char* texFile = node.getAttribute("tex");
	if (!texFile)
	{
		GHDebugMessage::outputString("error loading GHBulletTextureLookupFrictionOverride: no tex specified");
		return 0;
	}

	GHProperty priorKeepTextureData = extraData.getProperty(GHRenderProperties::GP_KEEPTEXTUREDATA);
	extraData.setProperty(GHRenderProperties::GP_KEEPTEXTUREDATA, 1);
	GHTexture* tex = (GHTexture*)mResourceCache.getCacheResource(texFile, &extraData);
	if (!tex)
	{
		GHDebugMessage::outputString("error loading GHBulletTextureLookupFrictionOverride: unable to load texture");
		return 0;
	}
	extraData.setProperty(GHRenderProperties::GP_KEEPTEXTUREDATA, priorKeepTextureData);

	GHBulletTextureLookupFrictionOverride::ColorToFrictionMap colorToFrictionMap;
	const GHXMLNode* mapParentNode = node.getChild("colorToFrictionMap", false);
	if (mapParentNode)
	{
		auto mapEntries = mapParentNode->getChildren();
		colorToFrictionMap.reserve(mapEntries.size());
		for (size_t i = 0; i < mapEntries.size(); ++i)
		{
			const GHXMLNode* entryNode = mapEntries[i];
			unsigned int color = 0;
			entryNode->readAttrUInt("color", color);

			float friction = 0;
			entryNode->readAttrFloat("friction", friction);

			colorToFrictionMap.push_back(GHBulletTextureLookupFrictionOverride::ColorToFrictionPair((unsigned char)color, friction));
		}
	}

	GHRect<float, 2> bounds(GHPoint2(0,0), GHPoint2(0,0));
	const GHXMLNode* boundsNode = node.getChild("bounds", false);
	if (boundsNode)
	{
		boundsNode->readAttrFloatArr("min", &bounds.mMin[0], 2);
		boundsNode->readAttrFloatArr("max", &bounds.mMax[0], 2);
	}

	GHBulletTextureLookupFrictionOverride* ret = new GHBulletTextureLookupFrictionOverride(tex, bounds, colorToFrictionMap);
	return ret;
}
