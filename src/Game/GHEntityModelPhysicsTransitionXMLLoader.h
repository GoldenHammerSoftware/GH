// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPhysicsSim;
class GHXMLObjFactory;

/*
 <transitions stateId="ES_ACTIVE">
    <entityPhysicsTransition/>
 </transitions>
 */
// expects to have an "entity" property passed in as an argument.
// expects that entity to have a "model" property
class GHEntityModelPhysicsTransitionXMLLoader : public GHXMLObjLoader
{
public:
    GHEntityModelPhysicsTransitionXMLLoader(GHPhysicsSim& physicsSim,
                                            const GHXMLObjFactory& objFactory);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    GHPhysicsSim& mPhysicsSim;
    const GHXMLObjFactory& mObjFactory;
};
