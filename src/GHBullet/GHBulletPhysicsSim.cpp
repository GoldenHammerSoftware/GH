// Copyright Golden Hammer Software
#include "GHBulletPhysicsSim.h"
#include "GHBulletBtInclude.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHBulletPhysicsObject.h"
#include <algorithm>
#include "GHPhysicsObject.h"
#include "GHBulletDebugDraw.h"
#include "GHBulletUtil.h"
#include "GHPhysicsCollisionData.h"
#include "GHMath/GHFloat.h"
#include <cassert>
#include "GHUtils/GHProfiler.h"

GHBulletPhysicsSim::GHBulletPhysicsSim(const GHTimeVal& timeVal, const GHPoint3& gravity)
: mTimeVal(timeVal)
, mDebugDraw(0)
, mFixedTimestep(1.f/45.f) //todo: let the game decide.
, mIsPaused(false)
{
    mCollisionConfiguration = new btDefaultCollisionConfiguration();
	mDispatcher = new btCollisionDispatcher(mCollisionConfiguration);
    mBroadphaseInterface = new btDbvtBroadphase();
    mConstraintSolver = new btSequentialImpulseConstraintSolver;
    mWorld = new btDiscreteDynamicsWorld(mDispatcher, mBroadphaseInterface, mConstraintSolver, mCollisionConfiguration);
    
    mWorld->setGravity(btVector3(gravity[0], gravity[1], gravity[2]));
    
    btGImpactCollisionAlgorithm::registerAlgorithm((btCollisionDispatcher*)mWorld->getDispatcher());

    //enableDebugDraw(true);
}

GHBulletPhysicsSim::~GHBulletPhysicsSim(void)
{
    delete mWorld;
    delete mDebugDraw;
    delete mConstraintSolver;
    delete mBroadphaseInterface;
    delete mDispatcher;
    delete mCollisionConfiguration;
}

void GHBulletPhysicsSim::update(void)
{
	GHPROFILEBEGIN("physics")

	updateFromGame();
    float timePassed = mTimeVal.getTimePassed();
    assert(!GHFloat::isNaN(timePassed));
    float timeLeft = timePassed;
    if (mIsPaused)
    {
        timeLeft = 0;
    }

	/*
	// approach that rounds game time to the nearest multiple of physics update
	// and bypasses bullet's partial frame interpolation.
	int numSteps = 0;
	float timeUsed = 0;
	while (timeUsed < timePassed)
	{
		numSteps++;
		timeUsed += mFixedTimestep;
	}
	float usedDiff = timeUsed - timePassed;
	if (usedDiff > mFixedTimestep / 2.0f)
	{
		numSteps--;
		timeUsed -= mFixedTimestep;
	}
	for (size_t i = 0; i < numSteps; ++i)
	{
		float timestep = std::min(mFixedTimestep, timeLeft);
		mWorld->stepSimulation(timestep, 1, mFixedTimestep);
		processCollisions();

		timeLeft -= timestep;
	}
	// end of the rounding approach
	*/

	/*
	// approach that uses bullet partial frame interpolation:
    while (timeLeft > 0)
    {
        float timestep = std::min(mFixedTimestep, timeLeft);
        mWorld->stepSimulation(timestep, 1, mFixedTimestep);
        processCollisions();
        
        timeLeft -= timestep;
    }
	// end of the bullet partial frame approach.
	*/

	// just let bullet handle all
	//*
	int numSteps = mWorld->stepSimulation(timeLeft, 20, mFixedTimestep);
	processCollisions();
	//*/

    if (timePassed > 0) {
        // on the first frame everything will be repositioned to 0 if we call this.
        updateToGame(numSteps > 0);
    }
    
    if (mDebugDraw)
    {
        mDebugDraw->drawWorld();
    }
    
    GHPROFILEEND("physics")
}

void GHBulletPhysicsSim::updateFromGame(void)
{
    size_t numObjsInWorld = mPhysicsObjectsInWorld.size();
    for (size_t i = 0; i < numObjsInWorld; ++i)
    {
        GHBulletPhysicsObject* obj = mPhysicsObjectsInWorld[i];
        obj->updateFromGame();
    }
}

void GHBulletPhysicsSim::updateToGame(bool physicsTicked)
{
    size_t numObjsInWorld = mPhysicsObjectsInWorld.size();
    for (size_t i = 0; i < numObjsInWorld; ++i)
    {
        GHBulletPhysicsObject* obj = mPhysicsObjectsInWorld[i];
        obj->updateToGame(physicsTicked);
    }
}

void GHBulletPhysicsSim::onActivate(void)
{
    
}

void GHBulletPhysicsSim::onDeactivate(void)
{
    if (mDebugDraw)
    {
        mDebugDraw->clear();
    }
}

void GHBulletPhysicsSim::addToSim(GHBulletPhysicsObject* ghObject, btRigidBody* rigidBody, short collisionLayer)
{
    assert(!rigidBody->isInWorld());
    if(rigidBody->isInWorld()) {
        return;
    }
    
    short collisionMask = mCollisionLayerMgr.getMaskForLayer(collisionLayer);
    mWorld->addRigidBody(rigidBody, collisionLayer, collisionMask);
    mPhysicsObjectsInWorld.push_back(ghObject);
}

void GHBulletPhysicsSim::removeFromSim(GHBulletPhysicsObject* ghObject, btRigidBody* rigidBody)
{
    mWorld->removeRigidBody(rigidBody);
    PhysObjList::iterator iter = std::find(mPhysicsObjectsInWorld.begin(), mPhysicsObjectsInWorld.end(), ghObject);
    if (iter != mPhysicsObjectsInWorld.end())
    {
        mPhysicsObjectsInWorld.erase(iter);
    }
}

void GHBulletPhysicsSim::addToSim(btTypedConstraint* constraint, bool disableCollisionBetweenBodies)
{
    mWorld->addConstraint(constraint, disableCollisionBetweenBodies);
}

void GHBulletPhysicsSim::removeFromSim(btTypedConstraint* constraint)
{
    mWorld->removeConstraint(constraint);
}

void GHBulletPhysicsSim::addToSim(btRaycastVehicle* vehicle)
{
    mWorld->addVehicle(vehicle);
}

void GHBulletPhysicsSim::removeFromSim(btRaycastVehicle* vehicle)
{
    mWorld->removeVehicle(vehicle);
}

void GHBulletPhysicsSim::addToSim(GHPhysicsObject& physicsObject)
{
    physicsObject.activate();
}

void GHBulletPhysicsSim::removeFromSim(GHPhysicsObject& physicsObject)
{
    physicsObject.deactivate();
}

void GHBulletPhysicsSim::pause(void)
{
    mIsPaused = true;
}

void GHBulletPhysicsSim::unpause(void)
{
    mIsPaused = false;
}

void GHBulletPhysicsSim::setGravity(const GHPoint3& gravity)
{
    mWorld->setGravity(btVector3(gravity[0], gravity[1], gravity[2]));
}

void GHBulletPhysicsSim::enableDebugDraw(bool enable)
{
    if (!mDebugDraw) {
        mDebugDraw = new GHBulletDebugDraw(mWorld);
    }
    if (enable) {
        mWorld->setDebugDrawer(mDebugDraw);
    }
    else {
        mWorld->setDebugDrawer(0);
        mDebugDraw->clear();
    }
}

bool GHBulletPhysicsSim::castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                                 int collisionLayer,
                                 GHPhysicsCollisionData::GHPhysicsCollisionObject& result) const
{
    if (rayStart == rayEnd) {
        return false;
    }

    btVector3 rayFromWorld(rayStart[0], rayStart[1], rayStart[2]);
    btVector3 rayToWorld(rayEnd[0], rayEnd[1], rayEnd[2]);
    btCollisionWorld::ClosestRayResultCallback callback(rayFromWorld, rayToWorld);
    callback.m_collisionFilterGroup = collisionLayer;
    callback.m_collisionFilterMask = mCollisionLayerMgr.getMaskForLayer(collisionLayer);
    mWorld->rayTest(rayFromWorld, rayToWorld, callback);
    
    if (callback.m_collisionObject != 0)
    {
        result.mObject = GHBulletUtil::getGHObjFromBTObj(callback.m_collisionObject);
        
        result.mHitPos = rayEnd;
        result.mHitPos -= rayStart;
        result.mHitPos *= callback.m_closestHitFraction;
        result.mHitPos += rayStart;
        
        result.mNormal.setCoords(callback.m_hitNormalWorld[0], callback.m_hitNormalWorld[1], callback.m_hitNormalWorld[2]);
        
        return true;
    }
    return false;
}

bool GHBulletPhysicsSim::castRay(const GHPoint3& rayStart, const GHPoint3& rayEnd,
                                 int collisionLayer,
                                 std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const
{
    btVector3 rayFromWorld(rayStart[0], rayStart[1], rayStart[2]);
    btVector3 rayToWorld(rayEnd[0], rayEnd[1], rayEnd[2]);
    btCollisionWorld::AllHitsRayResultCallback callback(rayFromWorld, rayToWorld);
    callback.m_collisionFilterGroup = collisionLayer;
    callback.m_collisionFilterMask = mCollisionLayerMgr.getMaskForLayer(collisionLayer);
    mWorld->rayTest(rayFromWorld, rayToWorld, callback);
    
    int numHits = callback.m_collisionObjects.size();
    results.resize(numHits);
    for (int i = 0; i < numHits; ++i)
    {
        const btCollisionObject* collisionObj = callback.m_collisionObjects[i];
        
        GHPhysicsCollisionData::GHPhysicsCollisionObject& result = results[i];
        result.mObject = GHBulletUtil::getGHObjFromBTObj(collisionObj);
        result.mHitPos = rayEnd;
        result.mHitPos -= rayStart;
        result.mHitPos *= callback.m_hitFractions[i];
        result.mHitPos += rayStart;
    }
    
    return numHits != 0;
}

bool GHBulletPhysicsSim::findObjectsInSphere(const GHPoint3& sphereCenter, float sphereRadius,
											 int collisionLayer,
											 std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results) const
{
	btCollisionObject testObject;
	btSphereShape sphereShape(sphereRadius);
	testObject.setCollisionShape(&sphereShape);
	testObject.setCollisionFlags(testObject.getCollisionFlags() | btCollisionObject::CF_NO_CONTACT_RESPONSE);
	testObject.getWorldTransform().setOrigin(btVector3(sphereCenter[0], sphereCenter[1], sphereCenter[2]));
	
	class ResultCallback : public btCollisionWorld::ContactResultCallback
	{
	public:
		ResultCallback(std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& results, btCollisionObject& testObject) : mResults(results), mTestObject(testObject) { }

		virtual	btScalar addSingleResult(btManifoldPoint& cp, const btCollisionObjectWrapper* colObj0Wrap, int partId0, int index0, const btCollisionObjectWrapper* colObj1Wrap, int partId1, int index1)
		{
			mResults.push_back(GHPhysicsCollisionData::GHPhysicsCollisionObject());
			GHPhysicsCollisionData::GHPhysicsCollisionObject& result = mResults.back();

			assert(colObj0Wrap->getCollisionObject() == &mTestObject || colObj1Wrap->getCollisionObject() == &mTestObject && "Error in GHBulletPhysicsSim::castSphere: Why did we get a result callback that didn't include our test object?");

			if (colObj0Wrap->getCollisionObject() == &mTestObject)
			{
				result.mObject = GHBulletUtil::getGHObjFromBTObj(colObj1Wrap->getCollisionObject());
				result.mHitPos.setCoords(cp.m_positionWorldOnB.x(), cp.m_positionWorldOnB.y(), cp.m_positionWorldOnB.z());
				result.mNormal.setCoords(cp.m_normalWorldOnB.x(), cp.m_normalWorldOnB.y(), cp.m_normalWorldOnB.z());
				++mNumHits;
			}
			else
			{
				result.mObject = GHBulletUtil::getGHObjFromBTObj(colObj0Wrap->getCollisionObject());
				result.mHitPos.setCoords(cp.m_positionWorldOnA.x(), cp.m_positionWorldOnA.y(), cp.m_positionWorldOnA.z());
				result.mNormal.setCoords(-cp.m_normalWorldOnB.x(), -cp.m_normalWorldOnB.y(), -cp.m_normalWorldOnB.z());
				++mNumHits;
			}
			return 1.0f;
		}

		std::vector<GHPhysicsCollisionData::GHPhysicsCollisionObject>& mResults;
		btCollisionObject& mTestObject;
		int mNumHits{ 0 };
	} resultCallback(results, testObject);

	resultCallback.m_collisionFilterGroup = collisionLayer;
	resultCallback.m_collisionFilterMask = mCollisionLayerMgr.getMaskForLayer(collisionLayer);
	mWorld->contactTest(&testObject, resultCallback);

	return resultCallback.mNumHits != 0;
}

void GHBulletPhysicsSim::processCollisions(void)
{
    //This will send, to every GHPhysicsCollisionCallback registered with the GHBulletPhysicsSim, 
    // up to one callback per contact manifold per frame
    // with the contact point generating the largest impulse.
    //It won't send callbacks for manifolds with no contact points and it won't send callbacks if both objects in the manifold are asleep.
    // We used to additionally filter by impulse amount, but this caused a bug in GHBowling where game logic depended on detecting certain collisions regardless of impulse magnitude.
    // It's now expected that game-level callbacks do their own impulse filtering if necessary.

    int numManifolds = mWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; ++i)
    {
        btPersistentManifold* contactManifold = mWorld->getDispatcher()->getManifoldByIndexInternal(i);
        int numContacts = contactManifold->getNumContacts();
        if (numContacts <= 0)
        {
            continue;
        }
        int bodyAActivationState = contactManifold->getBody0()->getActivationState();
        int bodyBActivationState = contactManifold->getBody1()->getActivationState();

        if (bodyAActivationState == ISLAND_SLEEPING
            || bodyBActivationState == ISLAND_SLEEPING)
        {
            continue;
        }

        GHBulletPhysicsObject* objA = GHBulletUtil::getGHObjFromBTObj(contactManifold->getBody0());
        GHBulletPhysicsObject* objB = GHBulletUtil::getGHObjFromBTObj(contactManifold->getBody1());

        float maxImpulse = -1;
        btManifoldPoint* chosenContactPoint = 0;
        for (int j = 0; j < numContacts; ++j)
        {
            btManifoldPoint& contactPt = contactManifold->getContactPoint(j);
            if (contactPt.getAppliedImpulse() > maxImpulse)
            {
                maxImpulse = contactPt.getAppliedImpulse();
                chosenContactPoint = &contactPt;
            }
        }
        
        GHPhysicsCollisionData data(objA, objB, maxImpulse);
        if (chosenContactPoint)
        {
            data.mLHS.mHitPos.setCoords(chosenContactPoint->m_positionWorldOnA[0],
                                        chosenContactPoint->m_positionWorldOnA[1],
                                        chosenContactPoint->m_positionWorldOnA[2]);
                
            data.mRHS.mHitPos.setCoords(chosenContactPoint->m_positionWorldOnB[0],
                                        chosenContactPoint->m_positionWorldOnB[1],
                                        chosenContactPoint->m_positionWorldOnB[2]);
        }
            
        dispatchCollisionCallbacks(data);
    }
}

void GHBulletPhysicsSim::enableCollisions(int lhsLayer, int rhsLayer, bool collide)
{
    //Bullet only supports 16 bit collision layers
    assert(!(lhsLayer & 0xFFFF0000));
    assert(!(rhsLayer & 0xFFFF0000));
    mCollisionLayerMgr.setCollisionWith(lhsLayer, rhsLayer, collide);
}

void GHBulletPhysicsSim::enableCollisionsWithAll(int layer, bool collide)
{
	assert(!(layer & 0xFFFF0000));
	if (collide)
	{
		mCollisionLayerMgr.enableCollisionWithAll(layer);
	}
	else
	{
		mCollisionLayerMgr.disableCollisionWithAll(layer);
	}
}
