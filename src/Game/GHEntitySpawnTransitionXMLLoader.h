// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHXMLObjFactory;

/*
 // if parentNode is specified and there was a parent entity, attach skeleton to that.
 <entitySpawn pos="0 1 0" activate="ES_ACTIVE" parentNode=tag_blah>
 <entity/>
 </entitySpawn>
*/
class GHEntitySpawnTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHEntitySpawnTransitionXMLLoader(const GHStringIdFactory& hashTable,
                                     const GHXMLObjFactory& objFactory);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    const GHStringIdFactory& mIdFactory;
    const GHXMLObjFactory& mObjFactory;
};
