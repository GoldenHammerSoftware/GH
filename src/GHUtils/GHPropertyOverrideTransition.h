#pragma once

#include "GHUtils/GHTransition.h"
#include "GHUtils/GHProperty.h"
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;

//A Transition that sets a property to a specified value in onActivate
// but then restores it to whatever it was before in onDeactivate
class GHPropertyOverrideTransition : public GHTransition
{
public:
	GHPropertyOverrideTransition(GHPropertyContainer& props, const GHIdentifier& propId, const GHProperty& value);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHIdentifier mPropId;
	GHProperty mPropVal;
	GHProperty mCachedProperty;
	GHPropertyContainer& mProps;
};
