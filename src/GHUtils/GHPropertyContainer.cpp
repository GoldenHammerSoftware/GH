// Copyright Golden Hammer Software
#include "GHPropertyContainer.h"

GHProperty& GHPropertyContainer::setProperty(GHIdentifier type, const GHProperty& val)
{
	GHProperty& ret = mProperties[type];
	ret = val;
	return ret;
}

const GHProperty& GHPropertyContainer::getProperty(const GHIdentifier& type) const
{
    PropertyMap::const_iterator findIter = mProperties.find(type);
    if (findIter == mProperties.end())
    {
		if (!mParentProperties)
		{
			return GHProperty::sInvalidProperty;
		}
		else
		{
			// recurse up the tree.
			return mParentProperties->getProperty(type);
		}
    }
    return findIter->second;
}
