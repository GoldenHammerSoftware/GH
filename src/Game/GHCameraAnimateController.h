#pragma once

#include "GHUtils/GHController.h"
#include "GHCamera.h"
#include <vector>

class GHTimeVal;
class GHTransition;

class GHCameraAnimateController : public GHController
{
public:
	GHCameraAnimateController(const GHTimeVal& timeVal, GHCamera& targetCam, size_t numExpectedWaypoints=0);
	virtual ~GHCameraAnimateController(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	//pass true if we only care about setting the camera's transform, 
	// ie inherit the FoV and other settings from the target camera's initial settings (or the previous waypoint)
	void addWaypoint(const GHCamera& destination, float timeSinceLastWaypoint, bool onlyAnimateTransform);

	void setOnEndTransition(GHTransition* onEndTransition);
	void setAnimationStartTime(float startTime) { mIntendedAnimationStartTime = startTime; }

private:

	struct Waypoint
	{
		GHCamera mDestination;
		GHPoint4 mCachedQuat;
		float mTimeSinceLastWaypoint{ 0 };
		float mTimeSinceAnimationStart{ 0 };
		bool mOnlyAnimateTransform{ true };
	};

	void setTargetCamera(const Waypoint& lhs, const Waypoint& rhs, float timeSinceAnimationStart);

	int findLowerBoundIndex(float timeSinceAnimStart, int lastFoundIndex) const;
	int findUpperBoundIndex(float timeSinceAnimStart, int lowerBoundIndex) const;

	float calcInterpolationPct(const Waypoint& lowerBound, const Waypoint& upperBound, float timeSinceAnimStart) const;

	void setTargetCamera(int lowerBoundIndex, int upperBoundIndex, float timeSinceAnimStart);
	int sanitizeIndex(int waypointIndex) const;

private:
	std::vector<Waypoint> mWaypoints;
	const GHTimeVal& mTimeVal;
	GHCamera& mTargetCam;
	float mIntendedAnimationStartTime{ 0 };
	float mAnimationStartTime{ 0 };
	float mAnimationTime{ 0 };
	int mLastLowerBoundIndex{ -1 };
	GHTransition* mOnEndTransition{ nullptr };
};
