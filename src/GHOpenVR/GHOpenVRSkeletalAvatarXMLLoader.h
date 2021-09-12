#pragma once

#include "GHXMLObjLoader.h"
#include "GHString/GHIdentifier.h"

class GHInputState;
class GHModel;
class GHTransform;
class GHStringIdFactory;

class GHOpenVRSkeletalAvatarXMLLoader : public GHXMLObjLoader
{
public:
	GHOpenVRSkeletalAvatarXMLLoader(const GHTransform& hmdOrigin, const GHStringIdFactory& idFactory,
									const GHInputState& inputState, int gamepadIndex);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;

private:
	GHModel* extractModel(GHPropertyContainer& extraData, const GHIdentifier& modelProp) const;
	bool getModelsAsSubmodels(GHModel*& leftHandModel, GHModel*& rightHandModel, GHPropertyContainer& extraData) const;
	bool getModelsSeparately(GHModel*& leftHandModel, GHModel*& rightHandModel, const GHXMLNode& node, GHPropertyContainer& extraData) const;

private:
	const GHTransform& mHMDOrigin;
	const GHStringIdFactory& mIdFactory;
	const GHInputState& mInputState;
	int mGamepadIndex;
};
