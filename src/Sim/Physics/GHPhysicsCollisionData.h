// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class GHPhysicsObject;

struct GHPhysicsCollisionData
{
    struct GHPhysicsCollisionObject
    {
		GHPhysicsObject* mObject{ 0 };
        GHPoint3 mHitPos;
        GHPoint3 mNormal;
        GHPhysicsCollisionObject(void)
        : mObject(0), mHitPos(0,0,0)
        { }
        GHPhysicsCollisionObject(GHPhysicsObject* object)
        : mObject(object), mHitPos(0,0,0)
        { }
    };
    GHPhysicsCollisionObject mLHS;
    GHPhysicsCollisionObject mRHS;
	float mImpulse{ 0 };
    
    GHPhysicsCollisionData(void)
    { }
    
    GHPhysicsCollisionData(GHPhysicsObject* lhs, GHPhysicsObject* rhs, float impulse)
    : mLHS(lhs), mRHS(rhs), mImpulse(impulse)
    { }

    struct ClosestDistancePredicate
    {
    public:
        ClosestDistancePredicate(const GHPoint3& origin) : mOrigin(origin) { }
        bool operator()(const GHPhysicsCollisionObject& lhs, const GHPhysicsCollisionObject& rhs) const
        {
            GHPoint3 diff = lhs.mHitPos;
            diff -= mOrigin;
            float lhsDistSqr = diff.lenSqr();
            diff = rhs.mHitPos;
            diff -= mOrigin;
            return lhsDistSqr < diff.lenSqr();
        }

    private:
        GHPoint3 mOrigin;
    };
};
