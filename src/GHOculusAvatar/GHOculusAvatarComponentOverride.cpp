#include "GHOculusAvatarComponentOverride.h"
#include "GHUtils/GHPropertyContainer.h"
#include "OVR_Avatar.h"
#include "GHMath/GHTransform.h"

GHOculusAvatarComponentOverride::GHOculusAvatarComponentOverride(const GHPropertyContainer& appProps)
	: mAppProps(appProps)
{

}

void GHOculusAvatarComponentOverride::addOverride(const GHIdentifier& propID, GHOculusUtil::AvatarComponentType type)
{
	mOverrides.push_back(Override());
	Override& o = mOverrides.back();
	o.mPropID = propID;
	o.mType = type;
}

void GHOculusAvatarComponentOverride::cacheComponents(ovrAvatar* avatar, const GHTransform& avatarWorldTransform)
{
	if (!mOverrides.size()) { return; }

	for (Override& o : mOverrides)
	{
		switch (o.mType)
		{
		case GHOculusUtil::LEFTHAND:
		{
			if (const ovrAvatarHandComponent * handComp = ovrAvatarPose_GetLeftHandComponent(avatar))
			{
				o.mComponent = handComp->renderComponent;
			}
			else
			{
				o.mComponent = nullptr;
			}
		}
		break;
		case GHOculusUtil::RIGHTHAND:
		{
			if (const ovrAvatarHandComponent * handComp = ovrAvatarPose_GetRightHandComponent(avatar))
			{
				o.mComponent = handComp->renderComponent;
			}
			else
			{
				o.mComponent = nullptr;
			}
		}
		break;
		default:
			assert(false);
		}
	}

	mInvertedAvatarWorldTrans = avatarWorldTransform;
	mInvertedAvatarWorldTrans.invert();
}

void GHOculusAvatarComponentOverride::getComponentTransform(const ovrAvatarComponent* component, GHTransform& outComponentTransform)
{
	for (Override& o : mOverrides)
	{
		if (o.mComponent == component)
		{
			const GHTransform* overrideTransform = (const GHTransform*)mAppProps.getProperty(o.mPropID);
			if (overrideTransform)
			{
				overrideTransform->mult(mInvertedAvatarWorldTrans, outComponentTransform);
				return;
			}
			break;
		}
	}

	//If we don't have an override, just use the transform from the component.
	GHOculusUtil::convertOvrAvatarTransformToGH(component->transform, outComponentTransform);
}
