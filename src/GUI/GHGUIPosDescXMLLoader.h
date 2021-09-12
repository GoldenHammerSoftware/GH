// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifierMap.h"

/* format:
 <guiPos xFill=FT_PCT yFill=FT_PIXELS align="0 0" offset="0 0" size="50 50" xSizeAlign=A_RIGHT ySizeAlign=A_BOTTOM/>
*/
class GHGUIPosDescXMLLoader : public GHXMLObjLoader
{
public:
    GHGUIPosDescXMLLoader(const GHIdentifierMap<int>& enumMap);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    const GHIdentifierMap<int>& mEnumMap;
};
