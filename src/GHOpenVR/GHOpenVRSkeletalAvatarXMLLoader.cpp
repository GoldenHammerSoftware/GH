#include "GHOpenVRSkeletalAvatarXMLLoader.h"
#include "GHOpenVRSkeletalAvatar.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHModel.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHBaseIdentifiers.h"
#include "GHXMLNode.h"

GHOpenVRSkeletalAvatarXMLLoader::GHOpenVRSkeletalAvatarXMLLoader(const GHTransform& hmdOrigin,
																 const GHStringIdFactory& idFactory,
																 const GHInputState& inputState, 
																 int gamepadIndex)
	: mHMDOrigin(hmdOrigin)
	, mIdFactory(idFactory)
	, mInputState(inputState)
	, mGamepadIndex(gamepadIndex)
{

}

void* GHOpenVRSkeletalAvatarXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHModel* leftHandModel = 0, * rightHandModel = 0;
	//if (!getModelsAsSubmodels(leftHandModel, rightHandModel, extraData)) {
	//	return 0;
	//}

	if (!getModelsSeparately(leftHandModel, rightHandModel, node, extraData)) {
		return 0;
	}

	const char* gestureMapFilename = node.getAttribute("gestureMap");

	const GHTransform* transform = (GHTransform*)extraData.getProperty(GHBaseIdentifiers::P_TRANSFORM);
	if (!transform) {
		transform = &mHMDOrigin;
	}

	return new GHOpenVRSkeletalAvatar(*leftHandModel, *rightHandModel, mInputState, mGamepadIndex, mIdFactory, *transform, gestureMapFilename);
}

GHModel* GHOpenVRSkeletalAvatarXMLLoader::extractModel(GHPropertyContainer& extraData, const GHIdentifier& modelProp) const
{
	GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
	if (!parentEnt) {
		GHDebugMessage::outputString("error in GHOculusAvatarXMLLoader: no parent entity.");
		return 0;
	}
	GHModel* parentModel = parentEnt->mProperties.getProperty(modelProp);
	return parentModel;
}

bool GHOpenVRSkeletalAvatarXMLLoader::getModelsAsSubmodels(GHModel*& leftHandModel, GHModel*& rightHandModel, GHPropertyContainer& extraData) const
{
	GHModel* model = extractModel(extraData, GHEntityHashes::MODEL);
	if (!model) {
		return false;
	}

	std::vector<GHModel*>& submodels = model->getSubmodels();
	if (submodels.size() != 2) {
		return false;
	}

	leftHandModel = submodels[0];
	rightHandModel = submodels[1];
	if (!leftHandModel || !rightHandModel) {
		return false;
	}
	return true;
}

bool GHOpenVRSkeletalAvatarXMLLoader::getModelsSeparately(GHModel*& leftHandModel, GHModel*& rightHandModel, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier leftHandProp, rightHandProp;
	node.readAttrIdentifier("leftHandModelProp", leftHandProp, mIdFactory);
	node.readAttrIdentifier("rightHandModelProp", rightHandProp, mIdFactory);

	leftHandModel = extractModel(extraData, leftHandProp);
	rightHandModel = extractModel(extraData, rightHandProp);

	return leftHandModel && rightHandModel;
}
