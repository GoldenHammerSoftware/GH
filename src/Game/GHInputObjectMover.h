// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHPointerChangeTracker.h"
#include <vector>

class GHTransform;
class GHInputState;
class GHTimeVal;

class GHInputObjectMover : public GHController
{
public:
    enum Command
    {
        OM_FORWARD=0,
        OM_BACK,
        OM_UP,
        OM_DOWN,
        OM_LEFT,
        OM_RIGHT,
		OM_TURNLEFT,
		OM_TURNRIGHT,
		OM_TURNUP,
		OM_TURNDOWN,
		OM_TILTLEFT,
		OM_TILTRIGHT,
        OM_MOUSEROTATE,
        OM_MOUSEONLYVERTICAL, // to prevent left/right from mouse input.
        OM_SPRINT,
		OM_SLOW,
		OM_PRINTTRANSFORM,
        OM_MAX,
    };
    
public:
    GHInputObjectMover(GHTransform& target,
                       const GHInputState& inputState,
                       const GHTimeVal& timeVal); 
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void) {}
    
    void addCommand(int key, Command command, bool isGamepad=false);
    void setMoveSpeed(float val) { mMoveSpeed = val; }
	void setRotSpeed(float val) { mRotSpeed = val; }
    void addJoystickMoveIndex(unsigned int joyId);
	void addJoystickRotateIndex(unsigned int joyId);

	void setFrameOfReference(const GHTransform* frameOfReference) { mFrameOfReference = frameOfReference; }

	void setLinearBounds(const GHPoint3& minBounds, const GHPoint3& maxBounds);

private:
	void restrictToBounds(GHPoint3& point);

private:
    float checkCommand(Command command) const;

	typedef std::vector<unsigned int> JoystickIdList;
	void applyJoysticks(const JoystickIdList& joyIds, GHPoint3& outDir);
    
private:
    GHTransform& mTarget;
	const GHTransform* mFrameOfReference;
    const GHInputState& mInputState;
    const GHTimeVal& mTimeVal;
    GHPointerChangeTracker mPointerTracker;
    GHPoint3 mCamRot;
    float mMoveSpeed;
	float mRotSpeed;
    
    std::vector<int> mCommandKeys[OM_MAX];
	std::vector<int> mGamepadCommandKeys[OM_MAX];
	JoystickIdList mMovementJoyIds;
	JoystickIdList mRotateJoyIds;

	GHPoint3 mMinBounds;
	GHPoint3 mMaxBounds;
	bool mHasBounds{ false };
};
