// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;

/* example
 <transformNode id=ModelTop rotation="0 0 0" translate="0 0 0" scale=1>
    <transformNode id=floorNode translate="0 0 -1" transform="1 0 0 0 0 1 0 0 0 0 1 0 0 0 0 1"/>
 </transformNode>
*/
class GHTransformNodeXMLLoader : public GHXMLObjLoader
{
public:
    GHTransformNodeXMLLoader(GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHStringIdFactory& mIdFactory;
};
