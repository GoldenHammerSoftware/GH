// Copyright 2010 Golden Hammer Software
#include "CGCharacterController.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHPhysicsObject.h"
#include "CGProperties.h"
#include "GHDebugMessage.h"
#include "GHMath.h"
#include "GHFloat.h"
#include "GHPi.h"

CGCharacterController::CGCharacterController(GHEntity& entity)
: mEntity(entity)
, mPhysicsObject(0)
{
    
}

void CGCharacterController::update(void)
{
    if (!mPhysicsObject) { return; }
    
    GHPoint3 velocity(0,0,0);
    
    if (mEntity.mProperties.getProperty(CGProperties::PP_HASDESTINATION))
    {
        GHPoint3* destination = mEntity.mProperties.getProperty(CGProperties::PP_DESTINATION);
        if (destination)
        {
            mPhysicsObject->getTransform().getTranslate(velocity);
            velocity *= -1.f;
            velocity += *destination;
            
            velocity.normalize();
            
            faceDirection(velocity);
            
            const float sOurSpeed = 2.f;
            velocity *= sOurSpeed;
            
            updateRadius();
        }
    }

    GHPoint3 currentVelocity;
    mPhysicsObject->getLinearVelocity(currentVelocity);
    velocity[1] = currentVelocity[1];
    
    mPhysicsObject->setLinearVelocity(velocity);
}

void CGCharacterController::faceDirection(const GHPoint3& dir)
{
    GHPoint3 pos;
    mPhysicsObject->getTransform().getTranslate(pos);
    GHTransform transform;
   
    GHPoint3 in(0,0,1);
    GHPoint3 crossProd;
    GHMath::cross(dir, in, crossProd);
    
    float angle;
    angle = ::asin(crossProd.length());
    
    if (dir*in > 0)
    {
        angle += PI;
        angle = -angle;
    }

    if (crossProd*GHPoint3(0,1,0) < 0)
    {
        angle = -angle;
    }   
    
    
    transform.becomeYRotation(angle);
    transform.setTranslate(pos);
    
    mPhysicsObject->manuallyPosition(transform);
}

void CGCharacterController::onActivate(void)
{
    mPhysicsObject = 0;
    
    GHModel* model = mEntity.mProperties.getProperty(GHEntityHashes::MODEL);
    if (model)
    {
        mPhysicsObject = model->getPhysicsObject();
    }
    
    calcCurrentPosition(mLastPosition);
    
    mEntity.mProperties.setProperty(CGProperties::PP_MOVERADIUS, mEntity.mProperties.getProperty(CGProperties::PP_DEFAULTMOVERADIUS));
    mEntity.mProperties.setProperty(CGProperties::PP_HITPOINTS, mEntity.mProperties.getProperty(CGProperties::PP_DEFAULTHITPOINTS));
    mEntity.mProperties.setProperty(CGProperties::PP_INITIATIVEPOINTS, mEntity.mProperties.getProperty(CGProperties::PP_DEFAULTINITIATIVEPOINTS));
}

void CGCharacterController::onDeactivate(void)
{
    
}

void CGCharacterController::calcCurrentPosition(GHPoint2& outPos)
{
    outPos.setCoords(0,0);
    GHPoint3 pos;
    if (mPhysicsObject)
    {
        mPhysicsObject->getTransform().getTranslate(pos);
        outPos.setCoords(pos[0], pos[2]);
    }
}

void CGCharacterController::updateRadius(void)
{
    float radius = mEntity.mProperties.getProperty(CGProperties::PP_MOVERADIUS);
    GHPoint2 currentPos;
    calcCurrentPosition(currentPos);
    GHPoint2 diff = currentPos;
    diff -= mLastPosition;
    mLastPosition = currentPos;
    
    radius = std::max(0.f, radius - diff.length());
    mEntity.mProperties.setProperty(CGProperties::PP_MOVERADIUS, radius);
}
