// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHInputStructs.h"
#include "GHPhysicsCollisionData.h"
#include "GHString/GHIdentifier.h"

class GHGUICanvasMgr;
class GHEntity;
class GHPhysicsSim;
class GHTimeVal;

//#define DEBUG_POINTER_INDICATOR_LOCATION 1
#ifdef DEBUG_POINTER_INDICATOR_LOCATION
#include "GHDebugDraw.h"
#endif

// Monitors a single poseable and puts a model at hit location.
class GHPointerIndicator : public GHController
{
public:
	GHPointerIndicator(const GHInputStructs::Poseable& poseable, unsigned int gamepadIdx, unsigned int poseableIdx,
					   const GHGUICanvasMgr* canvasMgr, const GHPhysicsSim* physicsSim, int collisionLayer, GHEntity* indicatorEnt, 
					   GHEntity* pointer3dEnt, const GHTimeVal* timeVal, const GHIdentifier& fadeProp);
	~GHPointerIndicator(void);

	virtual void update(void) override;
	virtual void onActivate(void) override;
	virtual void onDeactivate(void) override;

	bool getCurrentPosition(GHPoint3& outPos) const;

	void showIndicator(void);
	void hideIndicator(void);

	GHPhysicsCollisionData::GHPhysicsCollisionObject* getObjectOnPointer(void);

private:
	void positionIndicator(const GHPoint3& colPos);
	bool findHitPoint(GHPoint3& colPos);
	bool findHitPointCanvas(GHPoint3& colPos);
	bool findHitPointPhysics(GHPoint3& colPos);
	// return true when complete
	bool applyIndicatorFade(bool fadeIn) const;

private:
	GHPhysicsCollisionData::GHPhysicsCollisionObject mLastPhysicsResult;
	const GHInputStructs::Poseable& mPoseable;
	const GHGUICanvasMgr* mCanvasMgr{ nullptr };
	const GHPhysicsSim* mPhysicsSim{ nullptr };
	unsigned int mGamepadIdx{ 0 }; // if -1, use non-gamepad poseable (eg, head)
	unsigned int mPoseableIdx{ 0 };
	int mCollisionLayer{ 0 };
	GHEntity* mIndicatorEnt{ 0 };
	GHEntity* mPointer3dEnt{ 0 };
	const GHTimeVal* mTimeVal;
	const GHIdentifier mFadeProp;

	bool mIndicatorActive{ false };
	bool mPhysicsObjectOnPointerIsValid{ false };

	float mActivateTime{ 0 };
	bool mWasActive{ false };
	bool mFadingOut{ false };
	GHPoint3 mLastColPos;

#ifdef DEBUG_POINTER_INDICATOR_LOCATION
	DebugCross mDebugCross;
#endif
};

