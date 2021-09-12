// Copyright Golden Hammer Software
#include "GHGUISliderInputWhileSelected.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHMath/GHMath.h"
#include "GHInputState.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHKeyDef.h"

GHGUISliderInputWhileSelected::GHGUISliderInputWhileSelected(GHPropertyContainer& props, const GHInputState& inputState, const GHTimeVal& timeVal,
															 const GHIdentifier& prop, const GHPoint2& range, float speed, int dimension)
	: mProps(props)
	, mInputState(inputState)
	, mTimeVal(timeVal)
	, mProp(prop)
	, mRange(range)
	, mSpeed(speed)
	, mDimension(dimension)
{

}

void GHGUISliderInputWhileSelected::onActivate(void)
{

}

void GHGUISliderInputWhileSelected::onDeactivate(void)
{

}

void GHGUISliderInputWhileSelected::update(void)
{
	float direction = getDirection();
	if (direction)
	{
		float value = mProps.getProperty(mProp);
		value += mSpeed * direction * mTimeVal.getTimePassed();
		value = GHMath::clamp(mRange[0], mRange[1], value);
		mProps.setProperty(mProp, value);
	}
}

float GHGUISliderInputWhileSelected::getDirection(void) const
{
	if (!(mDimension == 0
		|| mDimension == 1)) //we only support dimensions 0 and 1
	{
		return 0;
	}

	float ret = 0;
	
	unsigned int numGamepads = mInputState.getNumGamepads();
	for (unsigned int g = 0; g < numGamepads; ++g)
	{
		const GHInputStructs::Gamepad& gamepad = mInputState.getGamepad(g);
		if (!gamepad.mActive) {
			continue;
		}

		if (mDimension == 0)
		{
			ret += gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_DPADRIGHT);
			ret -= gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_DPADLEFT);
		}
		if (mDimension == 1)
		{ //depending on what we want, these might be backwards
			ret += gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_DPADUP);
			ret -= gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_DPADDOWN);
		}

		size_t numJoysticks = gamepad.mJoySticks.size();
		for (size_t j = 0; j < numJoysticks; ++j)
		{
			ret += gamepad.mJoySticks[j][mDimension];
		}
	}

	ret = GHMath::clamp(-1.f, 1.f, ret);

	const float kDeadZoneThreshold = .5f;
	if (fabs(ret) < kDeadZoneThreshold) {
		ret = 0;
	}

	return ret;
}
