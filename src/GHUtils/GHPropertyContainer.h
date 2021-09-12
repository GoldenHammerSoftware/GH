// Copyright Golden Hammer Software
#pragma once

#include "GHProperty.h"
#include "GHString/GHIdentifier.h"
#include <map>

class GHPropertyContainer
{
public:
	GHProperty& setProperty(GHIdentifier type, const GHProperty& val);
	const GHProperty& getProperty(const GHIdentifier& type) const;
	void setParentProperties(const GHPropertyContainer* parent) { mParentProperties = parent; }

private:
    typedef std::map<GHIdentifier, GHProperty> PropertyMap;
    PropertyMap mProperties;
	const GHPropertyContainer* mParentProperties{ 0 };
};
