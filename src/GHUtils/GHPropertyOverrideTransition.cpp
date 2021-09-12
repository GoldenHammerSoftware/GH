#include "GHUtils/GHPropertyOverrideTransition.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyOverrideTransition::GHPropertyOverrideTransition(GHPropertyContainer& props, const GHIdentifier& propId, const GHProperty& value)
	: mPropId(propId)
	, mPropVal(value)
	, mProps(props)
{

}

void GHPropertyOverrideTransition::activate(void)
{
	mCachedProperty = mProps.getProperty(mPropId);
	mProps.setProperty(mPropId, mPropVal);
}

void GHPropertyOverrideTransition::deactivate(void)
{
	mProps.setProperty(mPropId, mCachedProperty);
}
