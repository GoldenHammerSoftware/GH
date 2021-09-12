// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainerXMLLoader;
class GHStringIdFactory;

/*
 <message id=CHANGEMAINSTATE>
    <prop name=state><enumProp val=BS_INPUT/></prop>
 </message>
*/
class GHMessageXMLLoader : public GHXMLObjLoader
{
public:
    GHMessageXMLLoader(const GHPropertyContainerXMLLoader& propLoader,
                       const GHStringIdFactory& stringHash);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    const GHPropertyContainerXMLLoader& mPropLoader;
    const GHStringIdFactory& mStringHash;
};
