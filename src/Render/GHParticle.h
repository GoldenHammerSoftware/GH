// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"

// a description of a single node in a particle system.
// there can be thousands of these, so this class needs to stay tight.
class GHParticle
{
public:
	GHParticle(void);
	~GHParticle(void);

	GHPoint3& getPosition(void) { return mPosition; }
	const GHPoint3& getPosition(void) const { return mPosition; }
	void setPosition(const GHPoint3& pos) { mPosition.setCoords(pos.getCoords()); }

	GHPoint3& getVelocity(void) { return mVelocity; }
	const GHPoint3& getVelocity(void) const { return mVelocity; }
	void setVelocity(const GHPoint3& val) { mVelocity.setCoords(val.getCoords()); }

	float getRadius(void) const { return mRadius; }
	void setRadius(const float rad) { mRadius = rad; }

	float getStartTime(void) const { return mStartTime; }
	void setStartTime(float val) { mStartTime = val; }

	float getEndTime(void) const { return mEndTime; }
	void setEndTime(float val) { mEndTime = val; }

	bool isDead(void) const { return mDead; }
	void setDead(bool val) { mDead = val; }

	float getDistFromCam(void) const { return mDistFromCamSqr; }
	void setDistFromCam(float val) { mDistFromCamSqr = val; }

	float getAlpha(void) const { return mAlpha; }
	void setAlpha(float val) { mAlpha = val; }
	void setInitialAlpha(float val) { mInitialAlpha = val; }

	float getCurrRotation(void) const { return mCurrRotation; }
	void setCurrRotation(float val) { mCurrRotation = val; }
	float getRotationMod(void) const { return mRotationMod; }
	void setRotationMod(float val) { mRotationMod = val; }

	void reset(void);

protected:
	GHPoint3 mPosition;
	GHPoint3 mVelocity;
	float mRadius;
	float mStartTime;
	float mEndTime;
	float mDistFromCamSqr;
	float mAlpha;
	float mInitialAlpha;
	float mCurrRotation;
	float mRotationMod;

	// flag to say whether this is an active particle or not.
	bool mDead;
};
