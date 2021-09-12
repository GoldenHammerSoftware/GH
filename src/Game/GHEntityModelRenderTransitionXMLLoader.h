// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHRenderGroup;
class GHStringIdFactory;

/*
 <transitions stateId="ES_ACTIVE">
    <entityRenderTransition renderType=RT_OBJECTS/>
 </transitions>
*/
// expects to have an "entity" property passed in as an argument.
// expects that entity to have a "model" property
class GHEntityModelRenderTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHEntityModelRenderTransitionXMLLoader(GHRenderGroup& renderGroup, GHStringIdFactory& hashTable);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHRenderGroup& mRenderGroup;
    GHStringIdFactory& mIdFactory;
};
