// Copyright Golden Hammer Software
#include "GHBulletUtil.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHFloat.h"

namespace GHBulletUtil
{
    bool isNaN(const btTransform& bulletTrans)
    {
        for(int i = 0; i < 3; ++i)
        {
            for (int j = 0; j < 3; ++j)
            {
                if (GHFloat::isNaN(bulletTrans.getBasis()[i][j]))
                {
                    return true;
                }
            }
        }
        
        for (int i = 0; i < 3; ++i)
        {
            if (GHFloat::isNaN(bulletTrans.getOrigin()[i]))
            {
                return true;
            }
        }
        return false;
    }
    
    void bulletToGHTransform(const btTransform& bulletTrans, GHTransform& ghTrans)
    {
        const btMatrix3x3& basis = bulletTrans.getBasis();
        
        for (unsigned int i = 0; i < 3; ++i)
        {
            for (unsigned int j = 0; j < 3; ++j)
            {
                ghTrans[i*4+j] = basis[j][i];
            }
        }
        
        const btVector3& origin = bulletTrans.getOrigin();
        GHPoint3 translate(origin.x(), origin.y(), origin.z());
        
        ghTrans.setTranslate(translate);
    }
    
    void ghToBulletTransform(const GHTransform& ghTrans, btTransform& bulletTrans)
    {
        btMatrix3x3& basis = bulletTrans.getBasis();
        for (unsigned int i = 0; i < 3; ++i)
        {
            for (unsigned int j = 0; j < 3; ++j)
            {
                basis[j][i] = ghTrans[i*4+j];
            }
        }
        
        GHPoint3 translate;
        ghTrans.getTranslate(translate);
        btVector3 origin(translate[0], translate[1], translate[2]);
        
        bulletTrans.setOrigin(origin);
    }
    
    GHBulletPhysicsObject* getGHObjFromBTObj(const btCollisionObject* collisionObject)
    {
        GHBulletPhysicsObject* ret = static_cast<GHBulletPhysicsObject*>(collisionObject->getUserPointer());
        return ret;
    }
}
