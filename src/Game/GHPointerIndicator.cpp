// Copyright Golden Hammer Software
#include "GHPointerIndicator.h"
#include "GHGUICanvasMgr.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHPhysicsSim.h"
#include "GHPhysicsObject.h"
#include "GHPlatform/GHTimeVal.h"

GHPointerIndicator::GHPointerIndicator(const GHInputStructs::Poseable& poseable, unsigned int gamepadIdx, unsigned int poseableIdx,
	const GHGUICanvasMgr* canvasMgr, const GHPhysicsSim* physicsSim, int collisionLayer, GHEntity* indicatorEnt, GHEntity* pointer3dEnt,
	const GHTimeVal* timeVal, const GHIdentifier& fadeProp)
	: mPoseable(poseable)
	, mGamepadIdx(gamepadIdx)
	, mPoseableIdx(poseableIdx)
	, mCanvasMgr(canvasMgr)
	, mPhysicsSim(physicsSim)
	, mCollisionLayer(collisionLayer)
	, mIndicatorEnt(indicatorEnt)
	, mPointer3dEnt(pointer3dEnt)
	, mTimeVal(timeVal)
	, mFadeProp(fadeProp)
{
}

GHPointerIndicator::~GHPointerIndicator(void)
{
	hideIndicator();
	if (mIndicatorEnt)
	{
		delete mIndicatorEnt;
	}

	if (mPointer3dEnt)
	{
		delete mPointer3dEnt;
	}
}

void GHPointerIndicator::update(void)
{
	bool pointerActive = mPoseable.mActive;
	GHPoint3 colPos;
	if (pointerActive)
	{
		pointerActive &= findHitPoint(colPos);
	}
	if (pointerActive)
	{
		if (!mWasActive && mTimeVal)
		{
			mWasActive = true;
			mActivateTime = mTimeVal->getTime();
		}
		showIndicator();
		positionIndicator(colPos);
		applyIndicatorFade(true);
		// store out last collision
		mLastColPos = colPos;
	}
	else
	{
		if (mWasActive)
		{
			mActivateTime = mTimeVal->getTime();
			mWasActive = false;
			mFadingOut = true;
		}
		if (mFadingOut)
		{
			positionIndicator(mLastColPos);
			if (applyIndicatorFade(false))
			{
				hideIndicator();
				mFadingOut = false;
			}
		}
	}
}

void GHPointerIndicator::onActivate(void)
{
	mIndicatorActive = false;
	mPhysicsObjectOnPointerIsValid = false;
	mFadingOut = false;
}

void GHPointerIndicator::onDeactivate(void)
{
	if (mIndicatorActive)
	{
		hideIndicator();
	}
}

void GHPointerIndicator::showIndicator(void)
{
	if (mIndicatorActive)
	{
		return;
	}
	mIndicatorActive = true;
	mFadingOut = false;

	if (mIndicatorEnt)
	{
		mIndicatorEnt->mStateMachine.setState(GHEntityHashes::ES_ACTIVE);
	}

	if (mPointer3dEnt)
	{
		mPointer3dEnt->mStateMachine.setState(GHEntityHashes::ES_ACTIVE);
	}
}

void GHPointerIndicator::hideIndicator(void)
{
	if (!mIndicatorActive)
	{
		return;
	}
	mIndicatorActive = false;

	if (mIndicatorEnt)
	{
		mIndicatorEnt->mStateMachine.setState(GHEntityHashes::ES_INACTIVE);
	}

	if (mPointer3dEnt)
	{
		mPointer3dEnt->mStateMachine.setState(GHEntityHashes::ES_INACTIVE);
	}
}

void GHPointerIndicator::positionIndicator(const GHPoint3& colPos)
{
	GHTransform posTransform;
	posTransform.becomeIdentity();
	posTransform.setTranslate(colPos);

	// position the hit point ent
	if (mIndicatorEnt)
	{
		GHModel* indicatorModel = mIndicatorEnt->mProperties.getProperty(GHEntityHashes::MODEL);
		assert(indicatorModel && indicatorModel->getSkeleton());
		GHTransform& modelPos = indicatorModel->getSkeleton()->getLocalTransform();
		modelPos = posTransform;
	}

	// position the line
	if (mPointer3dEnt)
	{
		GHPoint3 rayStart;
		mPoseable.mPosition.getTranslate(rayStart);

#ifdef DEBUG_POINTER_INDICATOR_LOCATION
		mDebugCross.setTranslate(rayStart);
#endif

		GHPoint3 distance = colPos;
		distance -= rayStart;

		GHModel* indicatorModel = mPointer3dEnt->mProperties.getProperty(GHEntityHashes::MODEL);
		assert(indicatorModel && indicatorModel->getSkeleton());
		GHTransform& modelPos = indicatorModel->getSkeleton()->getLocalTransform();
		//modelPos.becomeIdentity();
		modelPos = mPoseable.mPosition;

		GHTransform scaleMatrix;
		scaleMatrix.becomeScaleMatrix(1, 1, distance.length());
		scaleMatrix.mult(modelPos, modelPos);

		modelPos.setTranslate(rayStart);
	}
}

static void applyFadeProp(GHEntity* ent, float opacity, GHIdentifier fadeProp)
{
	if (!ent) return;
	const GHProperty& colorProp = ent->mProperties.getProperty(fadeProp);
	if (colorProp.isValid())
	{
		GHPoint4* color = colorProp.getVal<GHPoint4*>();
		(*color)[3] = opacity;
		//mIndicatorEnt->mProperties.setProperty(color);
	}
}

bool GHPointerIndicator::applyIndicatorFade(bool fadeIn) const
{
	if (!mTimeVal)
	{
		// no fade without time val.
		return true;
	}

	const float sFadeTime = 0.25f; // total length of fade
	const float sMaxFade = 0.75f; // most opaque value;
	float timeActive = std::min(sFadeTime, mTimeVal->getTime() - mActivateTime);
	float fadePct = timeActive / sFadeTime;
	if (!fadeIn)
	{
		fadePct = 1.0f - fadePct;
	}
	float opacity = sMaxFade * fadePct;

	applyFadeProp(mIndicatorEnt, opacity, mFadeProp);
	applyFadeProp(mPointer3dEnt, opacity, mFadeProp);

	return (timeActive >= mActivateTime);
}

bool GHPointerIndicator::getCurrentPosition(GHPoint3& outPos) const
{
	if (!mIndicatorActive) { return false; }
	if (!mIndicatorEnt) { return false; }
	GHModel* indicatorModel = mIndicatorEnt->mProperties.getProperty(GHEntityHashes::MODEL);
	assert(indicatorModel && indicatorModel->getSkeleton());
	if (!indicatorModel || !indicatorModel->getSkeleton())
	{
		return false;
	}

	GHTransform& modelPos = indicatorModel->getSkeleton()->getLocalTransform();
	modelPos.getTranslate(outPos);
	return true;
}

bool GHPointerIndicator::findHitPoint(GHPoint3& colPos)
{
	// possible todo: Always do both checks and compare hit distances so that we can have physics objects obscuring canvases and vice versa
	// for now: canvases always have priority (if this GHPointerIndicator supports canvases)
	if (findHitPointCanvas(colPos))
	{
		mPhysicsObjectOnPointerIsValid = false;
		return true;
	}
	if (findHitPointPhysics(colPos))
	{
		return true;
	}
	return false;
}


bool GHPointerIndicator::findHitPointCanvas(GHPoint3& colPos)
{
	if (!mCanvasMgr) {
		return false;
	}

	//Potential todo: support mGamepadIdx == -1 (ie, head poseable)
	bool hitCanvas = mCanvasMgr->getGamepadPoseableCollision(mGamepadIdx, mPoseableIdx, colPos);
	if (hitCanvas)
	{
		return true;
	}
	return false;
}

bool GHPointerIndicator::findHitPointPhysics(GHPoint3& colPos)
{
	if (!mPhysicsSim) {
		return false;
	}

	GHPoint3 rayStart;
	mPoseable.mPosition.getTranslate(rayStart);
	GHPoint3 rayDir(0, 0, 1);
	mPoseable.mPosition.multDir(rayDir, rayDir);
	
	if (rayStart.length() < 0.001f)
	{
		// something went really wrong.  poseable position is probably all 0s.
		return false;
	}

	GHPoint3 rayEnd = rayDir;
	rayEnd *= 1000.f;
	rayEnd += rayStart;

	if (mPhysicsSim->castRay(rayStart, rayEnd, mCollisionLayer, mLastPhysicsResult))
	{
		colPos = mLastPhysicsResult.mHitPos;
		mPhysicsObjectOnPointerIsValid = true;
		return true;
	}

	colPos = rayEnd;
	mPhysicsObjectOnPointerIsValid = false;
	return false;
}

GHPhysicsCollisionData::GHPhysicsCollisionObject* GHPointerIndicator::getObjectOnPointer(void)
{
	if (mPhysicsObjectOnPointerIsValid)
	{
		return &mLastPhysicsResult;
	}
	else
	{
		return 0;
	}
}

