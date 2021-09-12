#pragma once
#include "GHXMLObjLoader.h"

class GHResourceFactory;

class GHBulletTextureLookupFrictionOverrideXMLLoader : public GHXMLObjLoader
{
public:
	GHBulletTextureLookupFrictionOverrideXMLLoader(GHResourceFactory& resourceCache);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHResourceFactory& mResourceCache;
};
