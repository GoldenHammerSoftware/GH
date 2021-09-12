// Copyright Golden Hammer Software
#include "GHGUINotchedSliderNavResponse.h"
#include "GHUtils/GHPropertyContainer.h"
#include <cfloat>

GHGUINotchedSliderNavResponse::GHGUINotchedSliderNavResponse(GHPropertyContainer& props, const GHIdentifier& propID, const std::vector<float>& vals)
	: mProps(props)
	, mPropID(propID)
	, mVals(vals)
{

}

bool GHGUINotchedSliderNavResponse::respondToNavigation(const GHPoint2i& navDir)
{
	if (!mVals.size()) {
		return false;
	}

	float value = mProps.getProperty(mPropID);
	int index = findIndex(value);

	if (index < 0 || index >= (int)mVals.size()) {
		return true; //false? not sure what to do if the index is bad. this should never happen
	}

	if (navDir[0] > 0 || navDir[1] > 0)
	{
		if (index < (int)mVals.size() - 1) 
		{
			++index;
			mProps.setProperty(mPropID, mVals[index]);
		}
	}
	if (navDir[0] < 0 || navDir[1] < 0)
	{
		if (index > 0)
		{
			--index;
			mProps.setProperty(mPropID, mVals[index]);
		}
	}

	return true;
}

int GHGUINotchedSliderNavResponse::findIndex(float value) const
{
	float smallestDiff = FLT_MAX;
	int ret = -1;
	size_t numVals = mVals.size();
	for (size_t i = 0; i < numVals; ++i)
	{
		float diff = fabs(value - mVals[i]);
		if (diff < smallestDiff) {
			smallestDiff = diff;
			ret = (int)i;
		}
	}
	return ret;
}
