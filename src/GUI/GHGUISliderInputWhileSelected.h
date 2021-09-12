// Copyright Golden Hammer Software
#pragma once
#include "GHUtils/GHController.h"
#include "GHMath/GHPoint.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;
class GHInputState;
class GHTimeVal;

//how slider bars in the gui respond to controller/keyboard input
class GHGUISliderInputWhileSelected : public GHController
{
public:
	GHGUISliderInputWhileSelected(GHPropertyContainer& props, const GHInputState& inputState, const GHTimeVal& timeVal, 
								 const GHIdentifier& prop, const GHPoint2& range, float speed, int dimension);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	float getDirection(void) const;

private:
	GHPropertyContainer& mProps;
	const GHInputState& mInputState;
	const GHTimeVal& mTimeVal;
	GHIdentifier mProp;
	GHPoint2 mRange;
	float mSpeed;
	int mDimension;
};
