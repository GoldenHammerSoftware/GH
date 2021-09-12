// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHStringIdFactory;
class GHXMLObjFactory;
class GHControllerMgr;

// Loads a transition that when active activates/deactivates other transitions.
/*
 <transitionSwitch prop=GP_GUIDEARROWS>
    <switches>
        <switch>
            <properties>
                <boolProp val=false/>
            </properties>
            <transitions>
                <addModelTransition/>
            </transitions>
        </switch>
        <switch>
            <properties>
                <boolProp val=false/>
            </properties>
            <transitions>
                <addModelTransition/>
            </transitions>
        </switch>
    </switches>
 </transitionSwitch>
 */
class GHTransitionSwitchXMLLoader : public GHXMLObjLoader
{
public:
    GHTransitionSwitchXMLLoader(const GHPropertyContainer& props, const GHStringIdFactory& hashTable,
                                GHXMLObjFactory& objFactory, GHControllerMgr& controllerMgr);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;
    
private:
    const GHPropertyContainer& mProps;
    const GHStringIdFactory& mIdFactory;
    GHXMLObjFactory& mObjFactory;
    GHControllerMgr& mControllerMgr;
};
