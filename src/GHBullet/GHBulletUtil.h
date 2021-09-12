// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

class btTransform;
class GHTransform;
class GHBulletPhysicsObject;
class btCollisionObject;

namespace GHBulletUtil
{
    void bulletToGHTransform(const btTransform& bulletTrans, GHTransform& ghTrans);
    void ghToBulletTransform(const GHTransform& ghTrans, btTransform& bulletTrans);
    
    bool isNaN(const btTransform& bulletTrans);
    
    GHBulletPhysicsObject* getGHObjFromBTObj(const btCollisionObject* bulletObj);
}
