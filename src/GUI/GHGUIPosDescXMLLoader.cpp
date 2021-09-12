// Copyright Golden Hammer Software
#include "GHGUIPosDescXMLLoader.h"
#include "GHGUIPosDesc.h"
#include "GHXMLNode.h"
#include "GHMath/GHMath.h"

GHGUIPosDescXMLLoader::GHGUIPosDescXMLLoader(const GHIdentifierMap<int>& enumMap)
: mEnumMap(enumMap)
{
}

void* GHGUIPosDescXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIPosDesc* ret = new GHGUIPosDesc;
    populate(ret, node, extraData);
    return ret;
}

void GHGUIPosDescXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIPosDesc* ret = (GHGUIPosDesc*)obj;
    node.readAttrEnum("xFill", (int&)ret->mXFill, mEnumMap);
    node.readAttrEnum("yFill", (int&)ret->mYFill, mEnumMap);
    node.readAttrEnum("xSizeAlign", (int&)ret->mSizeAlign[0], mEnumMap);
    node.readAttrEnum("ySizeAlign", (int&)ret->mSizeAlign[1], mEnumMap);
    node.readAttrFloatArr("align", ret->mAlign.getArr(), 2);
    node.readAttrFloatArr("offset", ret->mOffset.getArr(), 2);
    node.readAttrFloatArr("size", ret->mSize.getArr(), 2);

    float angleDegrees = 0;
    if (node.readAttrFloat("angleDegrees", angleDegrees))
    {
        ret->mAngleRadians = GHMath::deg2Rad(angleDegrees);
    }
    else
    {
        node.readAttrFloat("angleRadians", ret->mAngleRadians);
    }
}

