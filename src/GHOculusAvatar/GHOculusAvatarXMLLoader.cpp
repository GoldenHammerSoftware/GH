#include "GHOculusAvatarXMLLoader.h"
#include "GHOculusAvatar.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHOculusAvatarPlatformPoser.h"
#include "GHBaseIdentifiers.h"

GHOculusAvatarXMLLoader::GHOculusAvatarXMLLoader(const char* appID,
												 uint64_t userID,
												 const GHXMLObjFactory& xmlObjFactory,
												 const GHTransform& hmdOrigin,
												 GHOculusAvatarPlatformPoserFactory& avatarPoserFactory,
												 GHVBFactory& vbFactory,
												 GHMaterialCallbackMgr& materialCallbackMgr,
												 const GHStringIdFactory& hashTable,
												 const GHInputState& inputState,
												 const GHPropertyContainer& appProps,
												 const GHXMLSerializer& xmlSerializer,
												 GHEventMgr& systemEventMgr, GHEventMgr& gameEventMgr,
												 GHResourceFactory& resourceFactory)
	: mUserID(userID)
	, mXMLObjFactory(xmlObjFactory)
	, mHMDOrigin(hmdOrigin)
	, mAvatarPoserFactory(avatarPoserFactory)
	, mVBFactory(vbFactory)
	, mMaterialCallbackMgr(materialCallbackMgr)
	, mIdFactory(hashTable)
	, mInputState(inputState)
	, mAppProps(appProps)
	, mXMLSerializer(xmlSerializer)
	, mSystemEventMgr(systemEventMgr)
	, mGameEventMgr(gameEventMgr)
	, mResourceFactory(resourceFactory)
{
	ovrAvatar_Initialize(appID);
}

GHOculusAvatarXMLLoader::~GHOculusAvatarXMLLoader(void)
{
	//shut down the avatar module 
	ovrAvatar_Shutdown();
}

void* GHOculusAvatarXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHModel* model = extractModel(extraData);
	if (!model) {
		return 0;
	}

	const char* matFilename = node.getAttribute("mat");
	if (!matFilename) {
		GHDebugMessage::outputString("Error in GHOculusAvatarXMLLoader: no mat specified");
		return 0;
	}

	const char* pbsMatFilename = node.getAttribute("pbsmat");
	if (!pbsMatFilename) {
		GHDebugMessage::outputString("Error in GHOculusAvatarXMLLoader: no pbsmat specified");
		return 0;
	}

	const GHTransform* transform = (GHTransform*)extraData.getProperty(GHBaseIdentifiers::P_TRANSFORM);
	if (!transform) {
		transform = &mHMDOrigin;
	}

	const char* gestureMapFilename = node.getAttribute("gestureMap");

	GHOculusAvatar* ret = new GHOculusAvatar(mUserID, *transform,
											 mAvatarPoserFactory.createAvatarPoser(), 
											 mVBFactory, mMaterialCallbackMgr, mIdFactory,
											 mInputState, mAppProps, mXMLSerializer, mXMLObjFactory, mResourceFactory, 
											 mSystemEventMgr, mGameEventMgr,
											 model, matFilename, pbsMatFilename, gestureMapFilename);


	loadComponentOverrides(node, ret);

	return ret;
}

void GHOculusAvatarXMLLoader::loadComponentOverrides(const GHXMLNode& node, GHOculusAvatar* ret) const
{
	if (!ret) { return; }

	GHOculusAvatarComponentOverride& componentOverride = ret->getComponentOverride();

	const GHXMLNode* overridesNode = node.getChild("componentOverrides", false);
	if (!overridesNode) return;

	for (const GHXMLNode* child : overridesNode->getChildren())
	{
		if (!child) { continue; }

		GHIdentifier propID;
		if (!child->readAttrIdentifier("prop", propID, mIdFactory)) {
			GHDebugMessage::outputString("Failed to load avatar component: no \"prop\" attribute");
			continue;
		}

		//A bit lazy here -- I didn't bother to make a GHENUM for this tiny list, so doing string compares by hand
		const char* idString = child->getAttribute("component");
		if (!idString) {
			GHDebugMessage::outputString("Failed to load avatar component: no \"component\" attribute");
			continue; 
		}
		GHOculusUtil::AvatarComponentType componentType;
		if (!strcmp(idString, "LEFTHAND"))
		{
			componentType = GHOculusUtil::LEFTHAND;
		}
		else if (!strcmp(idString, "RIGHTHAND"))
		{
			componentType = GHOculusUtil::RIGHTHAND;
		}
		else
		{
			GHDebugMessage::outputString("Failed to load avatar component override for unknown component %s", idString);
			continue;
		}

		componentOverride.addOverride(propID, componentType);
	}
}

GHModel* GHOculusAvatarXMLLoader::extractModel(GHPropertyContainer& extraData) const
{
	GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
	if (!parentEnt) {
		GHDebugMessage::outputString("error in GHOculusAvatarXMLLoader: no parent entity.");
		return 0;
	}
	GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
	if (!parentModel) 
	{
		parentModel = new GHModel;
		parentEnt->mProperties.setProperty(GHEntityHashes::MODEL, GHProperty(parentModel, parentModel));
	}

	return parentModel;
}

