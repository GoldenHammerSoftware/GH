// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHPropertyContainer;

/*
<!--propertyMap attribute: optional. if present, will look for the property map with the given identifier on extraData. Otherwise, default to app props passed to xml loader ctor-->

 <propertyToggle prop=GP_GUIDEARROWS propertyMap="PROPS"> 
    <boolProp val=false/>
    <boolProp val=true/>
 </propertyToggle>
*/
class GHPropertyToggleTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHPropertyToggleTransitionXMLLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
                                        GHPropertyContainer& props);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
    GHPropertyContainer& mProps;
};
