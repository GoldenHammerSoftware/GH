// Copyright Golden Hammer Software
#include "GHModelPhysicsTransition.h"
#include "GHModel.h"
#include "GHPhysicsObject.h"

GHModelPhysicsTransition::GHModelPhysicsTransition(GHModel& model, 
                                                   GHPhysicsSim& physicsSim,
                                                   GHPhysicsObject* replacementObj)
: mModel(model)
, mPhysicsSim(physicsSim)
, mReplacementObj(replacementObj)
, mOriginalObj(0)
{
}

GHModelPhysicsTransition::~GHModelPhysicsTransition(void)
{
    delete mReplacementObj;
}

void GHModelPhysicsTransition::activate(void)
{
    if (mReplacementObj)
    {
        mOriginalObj = mModel.getPhysicsObject();
        mModel.setPhysicsObject(mReplacementObj);
    }
    
    mModel.addPhysicsObjectToSim(mPhysicsSim);
}

void GHModelPhysicsTransition::deactivate(void)
{
    mModel.removePhysicsObjectFromSim(mPhysicsSim);
    
    if (mReplacementObj)
    {
        mModel.setPhysicsObject(mOriginalObj);
        mOriginalObj = 0;
    }
}
