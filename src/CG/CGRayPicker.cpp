// Copyright 2010 Golden Hammer Software
#include "CGRayPicker.h"
#include "GHPhysicsCollisionData.h"
#include "GHPhysicsSim.h"
#include "GHPhysicsObject.h"
#include "GHEntity.h"

CGRayPicker::CGRayPicker(const GHViewInfo& viewInfo, GHPhysicsSim& physicsSim)
: mPhysicsSim(physicsSim)
, mUnprojector(viewInfo)
, mGroundPhysics(0)
{
}

bool CGRayPicker::findGroundPos(const GHPoint2& screenPos, GHPoint3& worldPos) const
{
    if (!mGroundPhysics)
    {
        return false;
    }
    
    GHPoint3 rayOrigin, rayDir;
    mUnprojector.unproject(screenPos, rayOrigin, rayDir);
    
    rayDir *= 100.f;
    rayDir += rayOrigin;
    
    std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject> results;
    if (mPhysicsSim.castRay(rayOrigin, rayDir, results))
    {
        size_t numResults = results.size();
        for (size_t i = 0; i < numResults; ++i)
        {
            GHPhysicsCollisionData::GHPhysicsCollisionObject& result = results[i];
            if (result.mObject == mGroundPhysics)
            {
                worldPos = result.mHitPos;
                return true;
            }
        }
    }
    return false;
}

void CGRayPicker::findEntities(const GHPoint2& screenPos, std::vector<GHEntity*>& ret) const
{
    GHPoint3 rayOrigin, rayDir;
    mUnprojector.unproject(screenPos, rayOrigin, rayDir);
    
    rayDir *= 100.f;
    rayDir += rayOrigin;
    
    std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject> results;
    if (mPhysicsSim.castRay(rayOrigin, rayDir, results))
    {
        size_t numResults = results.size();
        for (size_t i = 0; i < numResults; ++i)
        {
            GHPhysicsCollisionData::GHPhysicsCollisionObject& result = results[i];
            GHEntity* ent = (GHEntity*)result.mObject->getUserData();
            ret.push_back(ent);
        }
    }
}
