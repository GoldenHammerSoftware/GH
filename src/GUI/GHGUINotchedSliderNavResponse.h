// Copyright Golden Hammer Software
#pragma once
#include "GHGUINavResponse.h"
#include "GHString/GHIdentifier.h"
#include <vector>

class GHPropertyContainer;

class GHGUINotchedSliderNavResponse : public GHGUINavResponse
{
public:
	GHGUINotchedSliderNavResponse(GHPropertyContainer& props, const GHIdentifier& propID, const std::vector<float>& vals);

	virtual bool respondToNavigation(const GHPoint2i& navDir);
	
private:
	int findIndex(float value) const;

private:
	GHPropertyContainer& mProps;
	GHIdentifier mPropID;
	std::vector<float> mVals;
};
