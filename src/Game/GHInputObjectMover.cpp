// Copyright Golden Hammer Software
#include "GHInputObjectMover.h"
#include "GHCamera.h"
#include "GHInputState.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHPoint.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHMath/GHFloat.h"
#include <algorithm>
#include "GHMath/GHMathDebugPrint.h"

GHInputObjectMover::GHInputObjectMover(GHTransform& target,
									const GHInputState& inputState,
									const GHTimeVal& timeVal)
: mTarget(target)
, mFrameOfReference(&target) //by default use input target as frame of reference
, mInputState(inputState)
, mTimeVal(timeVal)
, mPointerTracker(0, inputState)
, mCamRot(0,0,0)
, mMoveSpeed(1.0f)
, mRotSpeed(1.0f)
{
}

void GHInputObjectMover::update(void)
{
    GHPoint3 moveDir(0,0,0);
	moveDir[0] += checkCommand(OM_LEFT) - checkCommand(OM_RIGHT);
	moveDir[2] += checkCommand(OM_FORWARD) - checkCommand(OM_BACK);
	moveDir[1] += checkCommand(OM_UP) - checkCommand(OM_DOWN);
	applyJoysticks(mMovementJoyIds, moveDir);

    moveDir *= mTimeVal.getTimePassed();
    moveDir *= mMoveSpeed;
    if (checkCommand(OM_SPRINT)) {
        moveDir *= 10.0f;
    }
	if (checkCommand(OM_SLOW)) {
		moveDir *= 0.1f;
	}

	GHPoint3 rotDir(0, 0, 0);
	//todo: not sure if these should be reversed or not. feel free to change the signs here
	rotDir[1] += checkCommand(OM_TURNLEFT) - checkCommand(OM_TURNRIGHT);
	rotDir[0] += checkCommand(OM_TURNUP) - checkCommand(OM_TURNDOWN);
	rotDir[2] += checkCommand(OM_TILTLEFT) - checkCommand(OM_TILTRIGHT);
	applyJoysticks(mRotateJoyIds, rotDir);

	rotDir *= mTimeVal.getTimePassed();
	rotDir *= mRotSpeed;

	if (!checkCommand(OM_MOUSEROTATE))
	{
		mPointerTracker.clear();
	}
	else
	{
		GHPoint2 pointerChange;
		mPointerTracker.updatePointerChange(pointerChange);
		const float pointerScale = 3.0f;
		pointerChange *= pointerScale;

		if (!checkCommand(OM_MOUSEONLYVERTICAL))
		{
			rotDir[0] += pointerChange[0];
		}
		rotDir[1] += pointerChange[1]; 
		//rotDir[2] += 0; //pointer rotate only affects
	}

	if (mFrameOfReference)
	{
		//note: frame of reference only affects direction. 
		// Possible todo: make it *optionally* (defaulting to false)
		// also affect rotation
		mFrameOfReference->multDir(moveDir, moveDir);
	}

	GHPoint3 targPos;
	mTarget.getTranslate(targPos);

	if (!GHFloat::isZero(rotDir))
	{
		mCamRot += rotDir;

		GHTransform rotTrans;
		mTarget.becomeYRotation(-mCamRot[0]);
		rotTrans.becomeXRotation(mCamRot[1]);
		rotTrans.mult(mTarget, mTarget);

		GHTransform zRotTrans;
		zRotTrans.becomeZRotation(mCamRot[2]);
		zRotTrans.mult(mTarget, mTarget);
	}

    targPos += moveDir;
	restrictToBounds(targPos);
    mTarget.setTranslate(targPos);

	if (checkCommand(OM_PRINTTRANSFORM))
	{
		GHMathDebugPrint::printPoint(mTarget.getMatrix());
	}
}

void GHInputObjectMover::onActivate(void)
{
    // todo: grab starter values
}

void GHInputObjectMover::addCommand(int key, Command command, bool isGamepad)
{
	if (isGamepad)
	{
		mGamepadCommandKeys[command].push_back(key);
	}
	else
	{
		mCommandKeys[command].push_back(key);
	}
}

void GHInputObjectMover::addJoystickMoveIndex(unsigned int joyId)
{
	mMovementJoyIds.push_back(joyId);
}

void GHInputObjectMover::addJoystickRotateIndex(unsigned int joyId)
{
	mRotateJoyIds.push_back(joyId);
}

void GHInputObjectMover::setLinearBounds(const GHPoint3& minBounds, const GHPoint3& maxBounds)
{
	for (int i = 0; i < 3; ++i)
	{
		mMinBounds[i] = std::min(minBounds[i], maxBounds[i]);
		mMaxBounds[i] = std::max(minBounds[i], maxBounds[i]);
	}

	mHasBounds = true;
}

void GHInputObjectMover::restrictToBounds(GHPoint3& point)
{
	if (!mHasBounds) {
		return;
	}

	for (int i = 0; i < 3; ++i)
	{
		point[i] = std::min(std::max(point[i], mMinBounds[i]), mMaxBounds[i]);
	}
}


float GHInputObjectMover::checkCommand(Command command) const
{
	float ret = 0.0f;

    for (size_t i = 0; i < mCommandKeys[command].size(); ++i)
    {
		ret = std::max(ret, mInputState.getKeyState(0, mCommandKeys[command][i]));
    }

	for (unsigned int gpIdx = 0; gpIdx < mInputState.getNumGamepads(); ++gpIdx)
	{
		const auto& gamepad = mInputState.getGamepad(gpIdx);
		if (!gamepad.mActive) {
			continue;
		}
	
		for (size_t i = 0; i < mCommandKeys[command].size(); ++i)
		{
			ret = std::max(ret, gamepad.mButtons.getKeyState(mCommandKeys[command][i]));
		}
	}

    return ret;
}

void GHInputObjectMover::applyJoysticks(const JoystickIdList& joyIds, GHPoint3& outDir)
{
	for (size_t joyIdx = 0; joyIdx < joyIds.size(); ++joyIdx)
	{
		for (unsigned int gpIdx = 0; gpIdx < mInputState.getNumGamepads(); ++gpIdx)
		{
			if (!mInputState.getGamepad(gpIdx).mActive) {
				continue;
			}
			if (mInputState.getGamepad(gpIdx).mJoySticks.size() <= joyIds[joyIdx]) {
				continue;
			}
			const GHPoint2 joyPos = mInputState.getGamepad(gpIdx).mJoySticks[joyIds[joyIdx]];
			outDir[0] -= joyPos[0];
			outDir[2] += joyPos[1];
		}
	}
}

