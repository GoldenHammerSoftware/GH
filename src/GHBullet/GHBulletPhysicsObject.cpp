// Copyright Golden Hammer Software
#include "GHBulletPhysicsObject.h"
#include "GHBulletBtInclude.h"
#include "GHMath/GHTransform.h"
#include "GHBulletUtil.h"
#include "GHMath/GHFloat.h"
#include "GHBulletPhysicsSim.h"
#include "GHBulletRigidBodyCopier.h"
#include "GHBulletContactAddedFrictionOverride.h"

GHBulletPhysicsObject::GHBulletPhysicsObject(GHBulletPhysicsSim& physicsSim)
: mSim(physicsSim)
, mChangeHappened(false)
, mContactAddedFrictionOverride(0)
, mBTObj(0)
, mCollisionShape(0)
, mCollisionLayer(btBroadphaseProxy::DefaultFilter)
, debugging(false)
{
    mOffset.becomeIdentity();
    mOffsetInv.becomeIdentity();
}

void GHBulletPhysicsObject::setBulletObject(btRigidBody* rigidBody)
{
    delete mBTObj;
    mBTObj = rigidBody;
    mBTObj->setUserPointer(this);
    mBTObj->setCollisionFlags(mBTObj->getCollisionFlags() | btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK);
}

btRigidBody* GHBulletPhysicsObject::getBulletObject(void)
{
    return  mBTObj;
}

void GHBulletPhysicsObject::setCollisionShape(GHRefCountedType<btCollisionShape>* collisionShape)
{
    if (mCollisionShape) { mCollisionShape->release(); }
    mCollisionShape = collisionShape;
    mCollisionShape->acquire();
}

GHRefCountedType<btCollisionShape>* GHBulletPhysicsObject::getCollisionShape(void)
{
    return mCollisionShape;
}

void GHBulletPhysicsObject::setOffset(const GHTransform& offset)
{
    mOffset = offset;
    mOffsetInv = offset;
    mOffsetInv.invert();
}

GHBulletPhysicsObject::~GHBulletPhysicsObject(void)
{
    delete mBTObj;
    mCollisionShape->release();
	delete mContactAddedFrictionOverride;
}

void GHBulletPhysicsObject::applyOffset(GHTransform& outTransform)
{
    mOffset.mult(outTransform, outTransform);
}

void GHBulletPhysicsObject::applyInvOffset(GHTransform& outTransform)
{
    mOffsetInv.mult(outTransform, outTransform);
}

void GHBulletPhysicsObject::activate(void)
{
    GHTransform ourTrans = getTransform();
    applyOffset(ourTrans);
    
    btTransform bulletTrans;
    bulletTrans.setIdentity();
    GHBulletUtil::ghToBulletTransform(ourTrans, bulletTrans);
    
    mBTObj->getMotionState()->setWorldTransform(bulletTrans);
    mBTObj->setInterpolationWorldTransform(bulletTrans);
    mBTObj->setWorldTransform(bulletTrans);

    mSim.addToSim(this, mBTObj, mCollisionLayer);
    mIsInPhysicsSim = true;
}

void GHBulletPhysicsObject::deactivate(void)
{
    mIsInPhysicsSim = false;
    mSim.removeFromSim(this, mBTObj);
}

GHBulletPhysicsObject* GHBulletPhysicsObject::clone(GHTransformNode* skeleton)
{
    btRigidBody* newRigid = GHBulletRigidBodyCopier::cloneBody(mCollisionShape->get(), mBTObj);
    
    GHBulletPhysicsObject* ret = new GHBulletPhysicsObject(mSim);
    ret->setBulletObject(newRigid);
    ret->setCollisionShape(mCollisionShape);
    ret->setCollisionLayer(mCollisionLayer);
    ret->setOffset(mOffset);
	ret->setIsKinematic(mIsKinematic);
    
    ret->setTransformNode(skeleton);

	if (mContactAddedFrictionOverride) {
		ret->setContactAddedFrictionOverride(mContactAddedFrictionOverride->clone());
	}

	ret->debugging = debugging;
    
    return ret;
}

void GHBulletPhysicsObject::updateFromGame(void)
{
	if (mIsKinematic)
	{
		btTransform bulletTrans;
		GHBulletUtil::ghToBulletTransform(getTransform(), bulletTrans);
		mBTObj->getMotionState()->setWorldTransform(bulletTrans);
		return;
	}


    if (wasMotionReset())
    {
        mBTObj->setLinearVelocity(btVector3(0,0,0));
        mBTObj->setAngularVelocity(btVector3(0,0,0));

		// if we reset, we don't want to updateToGame until the next time an actual physics tick happens.
		mWaitingForPhysicsTickForGameUpdate = true;
    }
    
    const GHTransform* manualPosition = getManuallyPositionedTransform();
    if (manualPosition)
    {
        assert(!GHFloat::isNaN(manualPosition->getMatrix()));
        btTransform bulletTrans;
        
        GHTransform offsetTransform = *manualPosition;
        applyOffset(offsetTransform);
        
        GHBulletUtil::ghToBulletTransform(offsetTransform, bulletTrans);
        assert(!GHBulletUtil::isNaN(bulletTrans));
        mBTObj->setWorldTransform(bulletTrans);
        mChangeHappened = true;
		mWaitingForPhysicsTickForGameUpdate = true;
    }
    
    size_t numForces = getCurrentForces().size();
    for (size_t i = 0; i < numForces; ++i)
    {
        const GHPhysicsObject::Force& force = getCurrentForces()[i];
        assert(!GHFloat::isNaN(force.mForce));
        assert(!GHFloat::isNaN(force.mPosition));
        mBTObj->applyForce(btVector3(force.mForce[0], force.mForce[1], force.mForce[2]), 
                           btVector3(force.mPosition[0], force.mPosition[1], force.mPosition[2]));
        mChangeHappened = true;
    }
    
    size_t numImpulses = getCurrentImpulses().size();
    for (size_t i = 0; i < numImpulses; ++i)
    {
        const GHPhysicsObject::Force& impulse = getCurrentImpulses()[i];
        assert(!GHFloat::isNaN(impulse.mForce));
        assert(!GHFloat::isNaN(impulse.mPosition));
        mBTObj->applyImpulse(btVector3(impulse.mForce[0], impulse.mForce[1], impulse.mForce[2]), 
                           btVector3(impulse.mPosition[0], impulse.mPosition[1], impulse.mPosition[2]));
        mChangeHappened = true;
    }
    
    const GHPoint3& torque = getCurrentTorque();
    if (!GHFloat::isZero(torque))
    {
        assert(!GHFloat::isNaN(torque));
        mBTObj->applyTorque(btVector3(torque[0], torque[1], torque[2]));
        mChangeHappened = true;
    }
    const GHPoint3& torqueImpulse = getCurrentAngularImpulse();
    if (!GHFloat::isZero(torqueImpulse))
    {
        assert(!GHFloat::isNaN(torqueImpulse));
        mBTObj->applyTorqueImpulse(btVector3(torqueImpulse[0], torqueImpulse[1], torqueImpulse[2]));
        mChangeHappened = true;
    }
    
    if (mChangeHappened)
    {
        mBTObj->activate();
        mChangeHappened = false;
    }
    
    resetFrameData();
}

void GHBulletPhysicsObject::updateToGame(bool physicsTicked)
{
	if (!physicsTicked && mWaitingForPhysicsTickForGameUpdate)
	{
		// bullet seems to be doing prediction so we want to call updateToGame even if it throttled based on fixed update rate.
		// however when motion was reset we lose out on the position we set if we do this work with no bullet update.
		return;
	}
	mWaitingForPhysicsTickForGameUpdate = false;

	if (mIsKinematic)
	{
		return;
	}

    btTransform bulletTrans;
    mBTObj->getMotionState()->getWorldTransform(bulletTrans);
    assert(!GHBulletUtil::isNaN(bulletTrans));
    
    GHTransform ghTrans;
    ghTrans.becomeIdentity();
    GHBulletUtil::bulletToGHTransform(bulletTrans, ghTrans);
    assert(!GHFloat::isNaN(ghTrans.getMatrix()));
    applyInvOffset(ghTrans);
    setTransform(ghTrans);
}

void GHBulletPhysicsObject::getLinearVelocity(GHPoint3& linearVelocity) const
{
    const btVector3& bulletVel = mBTObj->getLinearVelocity();
    linearVelocity.setCoords(bulletVel[0], bulletVel[1], bulletVel[2]);
}

void GHBulletPhysicsObject::getAngularVelocity(GHPoint3& angularVelocity) const
{
    const btVector3& bulletVel = mBTObj->getAngularVelocity();
    angularVelocity.setCoords(bulletVel[0], bulletVel[1], bulletVel[2]);
}

void GHBulletPhysicsObject::setLinearVelocity(const GHPoint3& linearVelocity)
{
    mBTObj->setLinearVelocity(btVector3(linearVelocity[0], linearVelocity[1], linearVelocity[2]));
    mChangeHappened = true;
}

void GHBulletPhysicsObject::setAngularVelocity(const GHPoint3& angularVelocity)
{
    mBTObj->setAngularVelocity(btVector3(angularVelocity[0], angularVelocity[1], angularVelocity[2]));
    mChangeHappened = true;
}

float GHBulletPhysicsObject::getMass(void) const
{
	if (GHFloat::isZero(mBTObj->getInvMass())) {
		return 0;
	}
	return 1.f / mBTObj->getInvMass();
}

void GHBulletPhysicsObject::setFriction(float friction)
{
	mBTObj->setFriction(friction);
}

float GHBulletPhysicsObject::getFriction(void) const
{
	return mBTObj->getFriction();
}

void GHBulletPhysicsObject::setRestitution(float restitution)
{
    mBTObj->setRestitution(restitution);
}

float GHBulletPhysicsObject::getRestitution(void) const
{
    return mBTObj->getRestitution();
}

void GHBulletPhysicsObject::getCenterOfMass(GHPoint3& outCoM) const
{
    const btVector3 com = mBTObj->getCenterOfMassPosition();
    outCoM.setCoords(com.x(), com.y(), com.z());
}

void GHBulletPhysicsObject::setAngularFactor(const GHPoint3& angularFactor)
{
	mBTObj->setAngularFactor(btVector3(angularFactor[0], angularFactor[1], angularFactor[2]));
}

void GHBulletPhysicsObject::setCollisionLayer(short collisionLayer)
{
    if (mCollisionLayer == collisionLayer) {
        return;
    }

    mCollisionLayer = collisionLayer;
    if (mBTObj) 
    {
        if (mIsInPhysicsSim)
        {
            deactivate();
            activate();
        }
    }
}

