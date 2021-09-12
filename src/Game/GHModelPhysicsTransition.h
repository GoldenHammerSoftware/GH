// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHModel;
class GHPhysicsSim;
class GHPhysicsObject;

class GHModelPhysicsTransition : public GHTransition
{
public:
    GHModelPhysicsTransition(GHModel& model, GHPhysicsSim& physicsSim, GHPhysicsObject* replacementObj=0);
    ~GHModelPhysicsTransition(void);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHModel& mModel;
    GHPhysicsSim& mPhysicsSim;
    GHPhysicsObject* mReplacementObj;
    GHPhysicsObject* mOriginalObj;
};
