// Copyright Golden Hammer Software
#include "GHBulletVehicle.h"
#include "GHBulletPhysicsObject.h"
#include "GHBulletPhysicsSim.h"
#include "GHEntity.h"
#include "GHBulletBtInclude.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHBulletUtil.h"
#include "GHDebugDraw.h"

//#define DEBUGDRAWWHEELS

bool gWheelCreated = false;

GHBulletVehicle::GHBulletVehicle(GHBulletPhysicsSim& physicsSim,
                                 GHControllerMgr& controllerMgr,
                                 GHBulletPhysicsObject* chassis,
                                 const btRaycastVehicle::btVehicleTuning& tuning,
                                 const GHIdentifier& inContactProp,
                                 const GHIdentifier& disableProp,
                                 const GHIdentifier& wheelPointProp,
                                 const GHIdentifier& wheelPointNormalProp,
                                 float updatePriority)
: mPhysicsSim(physicsSim)
, mControllerMgr(controllerMgr)
, mInContactProp(inContactProp)
, mDisableProp(disableProp)
, mWheelPointProp(wheelPointProp)
, mWheelPointNormalProp(wheelPointNormalProp)
, mChassis (chassis)
, mTuning(tuning)
, mUpdater(createVehicle(physicsSim,
                         chassis,
                         tuning),
           inContactProp, disableProp, wheelPointProp, wheelPointNormalProp, *this)
, mConstraintIsInSim(false)
, mUpdatePriority(updatePriority)
{
	
    gWheelCreated = true;
    mBtVehicle->setCoordinateSystem(0, 1, 2);
    
    GHPoint3 mWheelDistFromCenter(0, .75, 0);
    
    btVector3 wheelDirectionCS0(0,-1,0);
    btVector3 wheelAxisCS(-1,0,0);
    float suspensionRestLength = tuning.m_suspensionCompression;
    float wheelRadius = 1;
    
    btVector3 connectionPointCS0(-mWheelDistFromCenter[0], -mWheelDistFromCenter[1], mWheelDistFromCenter[2]);
    mBtVehicle->addWheel(connectionPointCS0,
                         wheelDirectionCS0,
                         wheelAxisCS,
                         suspensionRestLength,
                         wheelRadius,
                         mTuning,
                         false);
   /*
    connectionPointCS0 = btVector3(mWheelDistFromCenter[0], -mWheelDistFromCenter[1], -mWheelDistFromCenter[2]);
    mBtVehicle->addWheel(connectionPointCS0,
                         wheelDirectionCS0,
                         wheelAxisCS,
                         suspensionRestLength,
                         wheelRadius,
                         mTuning,
                         false);
    
    
    connectionPointCS0 = btVector3(-mWheelDistFromCenter[0], -mWheelDistFromCenter[1], -mWheelDistFromCenter[2]);
    mBtVehicle->addWheel(connectionPointCS0,
                         wheelDirectionCS0,
                         wheelAxisCS,
                         suspensionRestLength,
                         wheelRadius,
                         mTuning,
                         false);
    
    connectionPointCS0 = btVector3(mWheelDistFromCenter[0], -mWheelDistFromCenter[1], -mWheelDistFromCenter[2]);
    mBtVehicle->addWheel(connectionPointCS0,
                         wheelDirectionCS0,
                         wheelAxisCS,
                         suspensionRestLength,
                         wheelRadius,
                         mTuning,
                         false);
     */
}

GHBulletVehicle::~GHBulletVehicle(void)
{
    delete mBtVehicle;
    delete mChassis;
}

btRaycastVehicle& GHBulletVehicle::createVehicle(GHBulletPhysicsSim& physicsSim, GHBulletPhysicsObject* chassis, const btRaycastVehicle::btVehicleTuning& tuning)
{
    btVehicleRaycaster* raycaster = new btDefaultVehicleRaycaster(&physicsSim.getBTWorld());
    mBtVehicle = new btRaycastVehicle(tuning, chassis->getBulletObject(), raycaster);
    //mBtVehicle->se
    return *mBtVehicle;
}

void GHBulletVehicle::activate(void)
{
    mChassis->setUserData(getUserData());
    mChassis->activate();
    addConstraintToSim();
    mControllerMgr.add(&mUpdater, mUpdatePriority);
}

void GHBulletVehicle::deactivate(void)
{
    mControllerMgr.remove(&mUpdater);
    removeConstraintFromSim();
    mChassis->deactivate();
}

bool GHBulletVehicle::addConstraintToSim(void)
{
    bool ret = false;
    if (!mConstraintIsInSim) {
        mPhysicsSim.addToSim(mBtVehicle);
        ret = true;
    }
    mConstraintIsInSim = true;
    return ret;
}

void GHBulletVehicle::removeConstraintFromSim(void)
{
    if (mConstraintIsInSim) {
        mPhysicsSim.removeFromSim(mBtVehicle);
    }
    mConstraintIsInSim = false;
}

GHPhysicsObject* GHBulletVehicle::clone(GHTransformNode* skeleton)
{
    GHBulletPhysicsObject* chassisClone = mChassis->clone(skeleton);
    return new GHBulletVehicle(mPhysicsSim, mControllerMgr, chassisClone, mTuning, mInContactProp, mDisableProp, mWheelPointProp, mWheelPointNormalProp, mUpdatePriority);
}

void GHBulletVehicle::getLinearVelocity(GHPoint3& linearVelocity) const
{
    mChassis->getLinearVelocity(linearVelocity);
}

void GHBulletVehicle::getAngularVelocity(GHPoint3& angularVelocity) const
{
    mChassis->getAngularVelocity(angularVelocity);
}

void GHBulletVehicle::setLinearVelocity(const GHPoint3& linearVelocity)
{
    mChassis->setLinearVelocity(linearVelocity);
}

void GHBulletVehicle::setAngularVelocity(const GHPoint3& angularVelocity)
{
    mChassis->setAngularVelocity(angularVelocity);
}

float GHBulletVehicle::getMass(void) const
{
    return mChassis->getMass();
}

void GHBulletVehicle::setFriction(float friction)
{
    mChassis->setFriction(friction);
}

float GHBulletVehicle::getFriction(void) const
{
    return mChassis->getFriction();
}

void GHBulletVehicle::setRestitution(float restitution)
{
    mChassis->setRestitution(restitution);
}

float GHBulletVehicle::getRestitution(void) const
{
    return mChassis->getRestitution();
}

void GHBulletVehicle::getCenterOfMass(GHPoint3& outCoM) const
{
    return mChassis->getCenterOfMass(outCoM);
}

void GHBulletVehicle::setAngularFactor(const GHPoint3& angularFactor)
{
	mChassis->setAngularFactor(angularFactor);
}

void GHBulletVehicle::setCollisionLayer(short collisionLayer)
{
    assert(false && "GHBulletVehicle::setCollisionLayer unimplemented and untested (note - GHBulletPhysicsObject adds/removes from sim. this might cause issues)");
}

void GHBulletVehicle::applyForce(const GHPoint3& force, const GHPoint3& position)
{
    mChassis->applyForce(force, position);
}

void GHBulletVehicle::applyImpulse(const GHPoint3& impulse, const GHPoint3& position)
{
    mChassis->applyImpulse(impulse, position);
}

void GHBulletVehicle::applyTorque(const GHPoint3& torque)
{
    mChassis->applyTorque(torque);
}

void GHBulletVehicle::applyAngularImpulse(const GHPoint3& impulse)
{
    mChassis->applyAngularImpulse(impulse);
}

void GHBulletVehicle::resetMotion(void)
{
    mChassis->resetMotion();
}

GHBulletVehicle::Updater::Updater(btRaycastVehicle& vehicle,
                                  const GHIdentifier& inContactProp,
                                  const GHIdentifier& disableProp,
                                  const GHIdentifier& wheelPointProp,
                                  const GHIdentifier& wheelPointNormalProp,
                                  GHBulletVehicle& parent)
: mVehicle(vehicle)
, mInContactProp(inContactProp)
, mDisableProp(disableProp)
, mWheelPointProp(wheelPointProp)
, mWheelPointNormalProp(wheelPointNormalProp)
, mParent(parent)
{
}

GHBulletVehicle::Updater::~Updater(void)
{

}

void GHBulletVehicle::Updater::onActivate(void)
{
#ifdef DEBUGDRAWWHEELS
    int numLines = mVehicle.getNumWheels() * 2;
    mDebugDraws.resize(numLines);
    for (int i = 0; i < numLines; ++i)
    {
        mDebugDraws[i] = GHDebugDraw::getSingleton().addLine(GHPoint3(0,0,0), GHPoint3(0,0,0), GHPoint3(0,0,0));
    }
#endif
}

void GHBulletVehicle::Updater::onDeactivate(void)
{
#ifdef DEBUGDRAWWHEELS
    int numLines = (int)mDebugDraws.size();
    for (int i = 0; i < numLines; ++i)
    {
        GHDebugDraw::getSingleton().removeShape(mDebugDraws[i]);
    }
    mDebugDraws.resize(0);
#endif

	int numWheels = mVehicle.getNumWheels();
	for (int i = 0; i < numWheels; ++i)
	{
		btWheelInfo& info = mVehicle.getWheelInfo(i);
		info.m_raycastInfo.m_isInContact = false;
	}
}

void GHBulletVehicle::Updater::update(void)
{
    GHPhysicsObject* ghPhysObj = (GHPhysicsObject*)mVehicle.getRigidBody()->getUserPointer();
    GHEntity* ghEnt = (GHEntity*)ghPhysObj->getUserData();
    if ((bool)ghEnt->mProperties.getProperty(mDisableProp)) {
        mParent.removeConstraintFromSim();
        ghEnt->mProperties.setProperty(mWheelPointProp, 0);
        ghEnt->mProperties.setProperty(mWheelPointNormalProp, 0);
        return;
    }
    if (!mParent.addConstraintToSim())
    {
#ifdef DEBUGDRAWWHEELS
        drawWheels();
#endif
    
        int numWheels = mVehicle.getNumWheels();
        for (int i = 0; i < numWheels; ++i)
        {
            //   mVehicle.applyEngineForce(100.f, i);
        }
        
        setInContactProp();
    }
}

void GHBulletVehicle::Updater::setInContactProp(void)
{
    if (mInContactProp.isNull()) {
        return;
    }
    
    bool inContact = false;
    int numWheels = mVehicle.getNumWheels();
    for (int i = 0; i < numWheels; ++i)
    {
        const btWheelInfo& info = mVehicle.getWheelInfo(i);
        if (info.m_raycastInfo.m_isInContact) {
            inContact = true;
            const btVector3& contactPt = info.m_raycastInfo.m_contactPointWS;
            mCurrentContactPoint.setCoords(contactPt[0], contactPt[1], contactPt[2]);
            
            const btVector3& contactNorm = info.m_raycastInfo.m_contactNormalWS;
            mCurrentContactNormal.setCoords(contactNorm[0], contactNorm[1], contactNorm[2]);
            break;
        }
    }
    
    GHPhysicsObject* ghPhysObj = (GHPhysicsObject*)mVehicle.getRigidBody()->getUserPointer();
    GHEntity* ghEnt = (GHEntity*)ghPhysObj->getUserData();
    
    ghEnt->mProperties.setProperty(mInContactProp, inContact);
    ghEnt->mProperties.setProperty(mWheelPointProp, &mCurrentContactPoint);
    ghEnt->mProperties.setProperty(mWheelPointNormalProp, &mCurrentContactNormal);
}

void GHBulletVehicle::Updater::drawWheels(void)
{
    int numWheels = mVehicle.getNumWheels();
    for (int i = 0; i < numWheels; ++i)
    {
        drawWheel(i);
    }
}

void GHBulletVehicle::Updater::drawWheel(int index)
{
    const btWheelInfo& info = mVehicle.getWheelInfo(index);

#define BULLETMULT
    
#ifdef BULLETMULT
    typedef btVector3 POINTTYPE;
#else
    typedef GHPoint3 POINTTYPE;
#endif
    
    POINTTYPE top(0, info.m_wheelsRadius, 0);
    POINTTYPE bottom(0, -info.m_wheelsRadius, 0);
    POINTTYPE left(0, 0, -info.m_wheelsRadius);
    POINTTYPE right(0, 0, info.m_wheelsRadius);
   
#ifdef BULLETMULT
    top = info.m_worldTransform * top;
    bottom = info.m_worldTransform * bottom;
    left = info.m_worldTransform * left;
    right = info.m_worldTransform * right;
#else
    GHTransform worldTrans;
    GHBulletUtil::bulletToGHTransform(info.m_worldTransform, worldTrans);
    
    worldTrans.mult(top, top);
    worldTrans.mult(bottom, bottom);
    worldTrans.mult(left, left);
    worldTrans.mult(right, right);
#endif
    
    GHPoint3 lhsPoint(top[0], top[1], top[2]);
    GHPoint3 rhsPoint(bottom[0], bottom[1], bottom[2]);
    GHDebugDraw::getSingleton().setLine(mDebugDraws[index*2], lhsPoint, rhsPoint, GHPoint3(0,0,0));
    lhsPoint.setCoords(left[0], left[1], left[2]);
    rhsPoint.setCoords(right[0], right[1], right[2]);
    GHDebugDraw::getSingleton().setLine(mDebugDraws[index*2+1], lhsPoint, rhsPoint, GHPoint3(0,0,0));
}


