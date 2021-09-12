// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;
class GHEntity;

/*
<entity>
    <properties>
        <prop name=model><modelProp file=kya.ghm/></prop>
    </properties>
    <stateMachine>
        <transitions stateId="ES_ACTIVE">
            <entityRenderTransition/>
        </transitions>
    </stateMachine>
</entity>
*/
class GHEntityXMLLoader : public GHXMLObjLoader
{
public:
    GHEntityXMLLoader(GHXMLObjFactory& objFactory);
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    virtual void populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
    void assignGUID(GHPropertyContainer& entProps, const GHPropertyContainer& extraData) const;
    
private:
    GHXMLObjFactory& mObjFactory;
    mutable int mNextEntId;
};
