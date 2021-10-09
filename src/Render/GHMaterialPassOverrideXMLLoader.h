// Copyright Golden Hammer Software
#pragma once

#include "Base/GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHStringIdFactory;

/*
 <materialPassOverrides>
    <po pass="shadow"><ghm blahblah/></po>
 </materialPassOverrides>
*/
class GHMaterialPassOverrideXMLLoader : public GHXMLObjLoader
{
public:
    GHMaterialPassOverrideXMLLoader(GHXMLObjFactory& objFactory,
                                    GHStringIdFactory& hashTable);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHXMLObjFactory& mXMLObjFactory;
    GHStringIdFactory& mIdFactory;
};
