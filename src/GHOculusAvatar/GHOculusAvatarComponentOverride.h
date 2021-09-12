#pragma once
#include <vector>
#include "GHString/GHIdentifier.h"
#include "GHOculusAvatarUtil.h"
#include "GHMath/GHTransform.h"

class GHPropertyContainer;

// This handles, optionally, overriding the transform of a given avatar component (eg, left hand, right hand). The override 
//  is expected to be a GHTransform* in the app property map indexed by the given prop id. This transform is expected to be in world space.
// If there is not a component override, then getComponentTransform will just return the transform given by the Oculus Avatar API
class GHOculusAvatarComponentOverride
{
public:
	GHOculusAvatarComponentOverride(const GHPropertyContainer& appProps);

	void addOverride(const GHIdentifier& propID, GHOculusUtil::AvatarComponentType type);

	void cacheComponents(ovrAvatar* avatar, const GHTransform& avatarWorldTransform);
	void getComponentTransform(const ovrAvatarComponent* component, GHTransform& outComponentTransform);

private:
	struct Override
	{
		GHIdentifier mPropID;
		GHOculusUtil::AvatarComponentType mType;
		const ovrAvatarComponent* mComponent{ nullptr };
	};

	std::vector<Override> mOverrides;

private:
	//Override transforms are expected to be in world space, but
	// the component transform needs to be in avatar space because it will ultimately
	// wind up affecting transforms that live inside the avatar skeleton, ie, subnodes of an avatar->worldspace transform.
	//Therefore, we need to make sure to transform our result into avatar space.
	GHTransform mInvertedAvatarWorldTrans; 
	const GHPropertyContainer& mAppProps;
};
