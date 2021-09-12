// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"
#include "GHPoint.h"

class GHEntity;
class GHPhysicsObject;

class CGCharacterController : public GHController
{
public:
    CGCharacterController(GHEntity& entity);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    void faceDirection(const GHPoint3& dir);
    
    void calcCurrentPosition(GHPoint2& outPos);
    void updateRadius(void);
    
private:
    GHEntity& mEntity;
    GHPhysicsObject* mPhysicsObject;
    GHPoint2 mLastPosition;
};
