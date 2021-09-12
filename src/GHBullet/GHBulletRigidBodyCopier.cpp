// Copyright Golden Hammer Software
#include "GHBulletRigidBodyCopier.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHFloat.h"

btRigidBody* GHBulletRigidBodyCopier::cloneBody(btCollisionShape* shape, const btRigidBody* src)
{
    btScalar mass = src->getInvMass();
    btVector3 localInertia(0,0,0);
    if (!GHFloat::isZero(mass))
    {
        mass = 1.f/mass;
        shape->calculateLocalInertia(mass, localInertia);
    }
    
    return cloneBody(shape, src, mass, localInertia);
}

btRigidBody* GHBulletRigidBodyCopier::cloneBody(btCollisionShape* shape, const btRigidBody* src, float mass, const btVector3& localInertia)
{
    btMotionState* motionState = new btDefaultMotionState;
    btRigidBody::btRigidBodyConstructionInfo info(mass, motionState, shape, localInertia);
    btRigidBody* ret = new btRigidBody(info);

    ret->setCcdMotionThreshold(src->getCcdMotionThreshold());
    ret->setCcdSweptSphereRadius(src->getCcdSweptSphereRadius());

    ret->setFriction(src->getFriction());
    ret->setRestitution(src->getRestitution());
    ret->setAngularFactor(src->getAngularFactor());
    ret->setLinearFactor(src->getLinearFactor());
    ret->setDamping(src->getLinearDamping(), src->getAngularDamping());
    ret->setCollisionFlags(src->getCollisionFlags());
    ret->setActivationState(src->getActivationState());

    return ret;
}

