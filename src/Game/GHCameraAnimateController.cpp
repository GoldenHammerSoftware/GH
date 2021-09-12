#include "GHCameraAnimateController.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHQuaternion.h"
#include "GHMath/GHFloat.h"
#include "GHUtils/GHTransition.h"

GHCameraAnimateController::GHCameraAnimateController(const GHTimeVal& timeVal, GHCamera& targetCam, size_t numExpectedWaypoints)
	: mTimeVal(timeVal)
	, mTargetCam(targetCam)
{
	//+1 for our phantom starting waypoint
	mWaypoints.reserve(numExpectedWaypoints + 1);

	//Insert a phantom waypoint that we will set to the camera's current position in onActivate.
	// This will be effectively skipped if the first waypoint's time is 0
	mWaypoints.push_back(Waypoint());
}

GHCameraAnimateController::~GHCameraAnimateController(void)
{
	delete mOnEndTransition;
}

void GHCameraAnimateController::onActivate(void)
{
	mAnimationTime = 0;
	mAnimationStartTime = mTimeVal.getTime() - mIntendedAnimationStartTime;
	mLastLowerBoundIndex = -1;

	mWaypoints[0].mDestination = mTargetCam;
	GHQuaternion::convertTransformToQuaternion(mTargetCam.getTransform().getMatrix(), mWaypoints[0].mCachedQuat);
}

void GHCameraAnimateController::onDeactivate(void)
{

}

void GHCameraAnimateController::setOnEndTransition(GHTransition* onEndTransition)
{
	delete mOnEndTransition;
	mOnEndTransition = onEndTransition;
}

void GHCameraAnimateController::update(void)
{
	float currentTime = mTimeVal.getTime();
	currentTime -= mAnimationStartTime;
	int lowerBoundIndex = findLowerBoundIndex(currentTime, mLastLowerBoundIndex);
	if (lowerBoundIndex == -1) {
		//This shouldn't happen unless time is negative because of the phantom start waypoint, 
		// but ensure that we always have a valid waypoint. 
		lowerBoundIndex = 0;
	}
	int upperBoundIndex = findUpperBoundIndex(currentTime, lowerBoundIndex);

	setTargetCamera(lowerBoundIndex, upperBoundIndex, currentTime);

	mAnimationTime = currentTime;

	if (mOnEndTransition && 
		(!mWaypoints.size()
		|| mAnimationTime > mWaypoints.back().mTimeSinceAnimationStart))
	{
		mOnEndTransition->activate();
	}
}

void GHCameraAnimateController::addWaypoint(const GHCamera& destination, float timeSinceLastWaypoint, bool onlyAnimateTransform)
{
	float prevTotalAnimTime = mWaypoints.back().mTimeSinceAnimationStart;
	mWaypoints.push_back(Waypoint());
	auto& wp = mWaypoints.back();
	wp.mDestination = destination;
	wp.mTimeSinceLastWaypoint = timeSinceLastWaypoint;
	wp.mTimeSinceAnimationStart = prevTotalAnimTime + timeSinceLastWaypoint;
	wp.mOnlyAnimateTransform = onlyAnimateTransform;
	GHQuaternion::convertTransformToQuaternion(destination.getTransform(), wp.mCachedQuat);
}

int GHCameraAnimateController::findLowerBoundIndex(float timeSinceAnimStart, int lastFoundIndex) const
{
	if (lastFoundIndex == -1) {
		lastFoundIndex = 0;
	}

	//Double check the last found index anyway in case the time hasn't changed enough.
	// If multiple waypoints have no time between them, we will skip to the last of them.
	int numTotalWaypoints = (int)mWaypoints.size();
	for (int i = lastFoundIndex; i < numTotalWaypoints; ++i)
	{
		if (mWaypoints[i].mTimeSinceAnimationStart > timeSinceAnimStart)
		{
			return i - 1;
		}
	}

	return numTotalWaypoints - 1;
}

int GHCameraAnimateController::findUpperBoundIndex(float timeSinceAnimStart, int lowerBoundIndex) const
{
	//If we've reached the end of the animation, don't go any higher.
	if (lowerBoundIndex == ((int)mWaypoints.size()) - 1)
	{
		return lowerBoundIndex;
	}

	//If for whatever reason our lower bound index is still too high, also use it as the upper bound
	if (lowerBoundIndex >= 0 && mWaypoints[lowerBoundIndex].mTimeSinceAnimationStart > timeSinceAnimStart)
	{
		return lowerBoundIndex;
	}

	return lowerBoundIndex + 1;
}

int GHCameraAnimateController::sanitizeIndex(int waypointIndex) const
{
	if (waypointIndex < 0) return 0;
	if (waypointIndex >= (int)mWaypoints.size()) return (int)(mWaypoints.size() - 1);
	return waypointIndex;
}

void GHCameraAnimateController::setTargetCamera(int lowerBoundIndex, int upperBoundIndex, float timeSinceAnimStart)
{
	if(!mWaypoints.size()) {
		return;
	}

	lowerBoundIndex = sanitizeIndex(lowerBoundIndex);
	upperBoundIndex = sanitizeIndex(upperBoundIndex);

	if (lowerBoundIndex == upperBoundIndex)
	{
		const auto& destination = mWaypoints[upperBoundIndex];
		if (destination.mOnlyAnimateTransform)
		{
			mTargetCam.getTransform() = destination.mDestination.getTransform();
		}
		else
		{
			mTargetCam = destination.mDestination;
		}
		return;
	}

	const auto& lowerBound = mWaypoints[lowerBoundIndex];
	const auto& upperBound = mWaypoints[upperBoundIndex];

	float pct = calcInterpolationPct(lowerBound, upperBound, timeSinceAnimStart);

	
	GHPoint4 interpolatedQuat;
	GHQuaternion::slerp(lowerBound.mCachedQuat, upperBound.mCachedQuat, pct, interpolatedQuat);
	GHQuaternion::convertQuaternionToTransform(interpolatedQuat, mTargetCam.getTransform().getMatrix());

	GHPoint3 lowerBoundPos;
	lowerBound.mDestination.getTransform().getTranslate(lowerBoundPos);

	GHPoint3 upperBoundPos;
	upperBound.mDestination.getTransform().getTranslate(upperBoundPos);

	GHPoint3 interpolatedPos;
	lowerBoundPos.linearInterpolate(upperBoundPos, pct, interpolatedPos);
	mTargetCam.getTransform().setTranslate(interpolatedPos);

	//I'm open to restricting what we interpolate here (or changing how we configure it).
	// In practice we may want to set specific values (like maybe FoV and near plane) but let everything
	// else be set by app configuration (which might change, ie as the window size changes).
	// Since current use cases mOnlyAnimateTransform=true it's moot as yet.
	if (!upperBound.mOnlyAnimateTransform)
	{
		mTargetCam.setFOV(GHFloat::lerp(lowerBound.mDestination.getFOV(), upperBound.mDestination.getFOV(), pct));
		mTargetCam.setNearClip(GHFloat::lerp(lowerBound.mDestination.getNearClip(), upperBound.mDestination.getNearClip(), pct));
		mTargetCam.setFarClip(GHFloat::lerp(lowerBound.mDestination.getFarClip(), upperBound.mDestination.getFarClip(), pct));
		mTargetCam.setFrustumFudge(GHFloat::lerp(lowerBound.mDestination.getFrustumFudge(), upperBound.mDestination.getFrustumFudge(), pct));

		bool orthoIsSame = (lowerBound.mDestination.getIsOrtho() == upperBound.mDestination.getIsOrtho());
		bool useLeftOrtho = pct < 0.5f;
		mTargetCam.setIsOrtho(useLeftOrtho ? lowerBound.mDestination.getIsOrtho() : upperBound.mDestination.getIsOrtho());
		if (orthoIsSame)
		{
			mTargetCam.setOrthoWidth(GHFloat::lerp(lowerBound.mDestination.getOrthoWidth(), upperBound.mDestination.getOrthoWidth(), pct));
			mTargetCam.setOrthoHeight(GHFloat::lerp(lowerBound.mDestination.getOrthoHeight(), upperBound.mDestination.getOrthoHeight(), pct));
		}
		else //We can't interpolate between is ortho or not, and the non-ortho side probably has bogus values, so just snap to destination
		{
			mTargetCam.setOrthoWidth(useLeftOrtho ? lowerBound.mDestination.getOrthoWidth() : upperBound.mDestination.getOrthoWidth());
			mTargetCam.setOrthoHeight(useLeftOrtho ? lowerBound.mDestination.getOrthoHeight() : upperBound.mDestination.getOrthoHeight());
		}

		GHPoint4 viewportClip;
		lowerBound.mDestination.getViewportClip().linearInterpolate(upperBound.mDestination.getViewportClip(), pct, viewportClip);
		mTargetCam.setViewportClip(viewportClip);
	}
}

float GHCameraAnimateController::calcInterpolationPct(const Waypoint& lowerBound, const Waypoint& upperBound, float timeSinceAnimStart) const
{
	float timeSinceLowerBound = timeSinceAnimStart - lowerBound.mTimeSinceAnimationStart;
	if (timeSinceLowerBound < 0) {
		return 0;
	}

	if (GHFloat::isZero(upperBound.mTimeSinceLastWaypoint)) {
		return 1.0;
	}

	return timeSinceLowerBound / upperBound.mTimeSinceLastWaypoint;
}
