#include "GHOculusAvatarCustomGestureSetter.h"
#include "GHOculusAvatar.h"
#include "GHUtils/GHEventMgr.h"
#include "GHXMLSerializer.h"
#include "GHXMLNode.h"
#include "GHGameIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHBaseIdentifiers.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHHMDIdentifiers.h"
#include "GHUtils/GHUtilsIdentifiers.h"

GHOculusAvatarCustomGestureSetter::GHOculusAvatarCustomGestureSetter(ovrAvatar*& avatar, 
																	 GHEventMgr& eventMgr,
																	 const GHXMLSerializer& xmlSerializer,
																	 const GHStringIdFactory& hashTable,
																	 const char* configFilename)
	: mAvatar(avatar)
	, mEventMgr(eventMgr)
	, mXMLSerializer(xmlSerializer)
	, mIdFactory(hashTable)
	, mDeviceReinitListener(eventMgr, *this)
{
	mEventMgr.registerListener(GHGameIdentifiers::M_SETHANDGESTURE, *this);
	mEventMgr.registerListener(GHHMDIdentifiers::M_SETAVATARCAPABILITY, *this);
	mEventMgr.registerListener(GHGameIdentifiers::M_RESETCURRENTHANDGESTURE, *this);
	loadGestureMap(configFilename);
}

GHOculusAvatarCustomGestureSetter::~GHOculusAvatarCustomGestureSetter(void)
{
    mEventMgr.unregisterListener(GHGameIdentifiers::M_RESETCURRENTHANDGESTURE, *this);
	mEventMgr.unregisterListener(GHHMDIdentifiers::M_SETAVATARCAPABILITY, *this);
	mEventMgr.unregisterListener(GHGameIdentifiers::M_SETHANDGESTURE, *this);

	auto iter = mGestureMap.begin(), iterEnd = mGestureMap.end();
	for (; iter != iterEnd; ++iter)
	{
		delete iter->second;
	}
}

void GHOculusAvatarCustomGestureSetter::handleMessage(const GHMessage& message)
{
	if (message.getType() == GHHMDIdentifiers::M_SETAVATARCAPABILITY)
	{
		handleSetCapabilities(message);
	}
	else if (message.getType() == GHGameIdentifiers::M_SETHANDGESTURE)
	{
		handleSetHandGesture(message);
	}
	else if (message.getType() == GHGameIdentifiers::M_RESETCURRENTHANDGESTURE)
	{
		handleResetCurrentHandGesture();
	}
}

void GHOculusAvatarCustomGestureSetter::handleSetHandGesture(const GHMessage& message)
{
	const GHProperty& propIdProp = message.getPayload().getProperty(GHUtilsIdentifiers::PROPID);
	GHIdentifier propId = 0;
	if (propIdProp.isValid())
	{
		propId = *((GHIdentifier*)propIdProp);
		setGesture(propId);
	}
}

void GHOculusAvatarCustomGestureSetter::handleSetCapabilities(const GHMessage& message)
{
	//todo: expand to call ovrAvatar_SetActiveCapabilities if we want to,
	//but note: capabilities must be a *subset* of the capabilities the avatar was initialized with
	
	const GHProperty& controllersProp = message.getPayload().getProperty(GHHMDIdentifiers::MP_CONTROLLERS);
	if (controllersProp.isValid())
	{
		bool controllersVisible = controllersProp;
		ovrAvatar_SetLeftControllerVisibility(mAvatar, controllersVisible);
		ovrAvatar_SetRightControllerVisibility(mAvatar, controllersVisible);
	}
}

void GHOculusAvatarCustomGestureSetter::loadGestureMap(const char* configFilename)
{
	GHXMLNode* node = mXMLSerializer.loadXMLFile(configFilename);
	if (!node) { return; }

	std::vector<GHString> strList;
	GHIdentifier id;

	auto& children = node->getChildren();
	size_t numChildren = children.size();
	for (size_t i = 0; i < numChildren; ++i)
	{
		const GHXMLNode* gestureNode = children[i];
		if (!gestureNode->readAttrIdentifier("id", id, mIdFactory)) {
			continue;
		}

		bool isDefault = false;
		gestureNode->readAttrBool("default", isDefault);

		if (!isDefault)
		{
			if (!gestureNode->readAttrStrList("file", strList)
				|| !strList.size())
			{
				continue;
			}
		}

		int handIndex = 0;
		gestureNode->readAttrInt("handIndex", handIndex);

		Gesture* gesture = new Gesture;
		mGestureMap[id] = gesture;
		gesture->mIsDefault = isDefault;
		gesture->mComponentType = handIndex == 0 ? GHOculusUtil::LEFTHAND : GHOculusUtil::RIGHTHAND;
		if (!isDefault)
		{
			gesture->mFilename = strList[0];
		}

		strList.resize(0);
	}

	delete node;
}

void GHOculusAvatarCustomGestureSetter::onDeviceReinit(void)
{
	// This is kind of stupid:
	// (this has so far been observed only on Oculus Go)
	// When the device reinits, if the hand is set to the default gesture, 
	// the Avatar API will start sending us NaNs in the transforms.
	// It's not enough to set the default gesture again, and it's also not enough
	// to set a different gesture and then immediately set the default gesture.
	// But if we set a concrete gesture (any will do, using GripSphere arbitrarily), 
	// then set it back to the default gesture after a frame has passed (via message sent from GHOculusAvatar),
	// the NaNs go away and the hand looks right.

	if (mAvatar)
	{
		ovrAvatar_SetLeftHandGesture(mAvatar, ovrAvatarHandGesture_GripSphere);
		ovrAvatar_SetRightHandGesture(mAvatar, ovrAvatarHandGesture_GripSphere);
	}
}

void GHOculusAvatarCustomGestureSetter::handleResetCurrentHandGesture(void)
{
	if (mCurrentGestures[GHOculusUtil::LEFTHAND])
	{
		setGesture(*mCurrentGestures[GHOculusUtil::LEFTHAND]);
	}
	else
	{
		ovrAvatar_SetLeftHandGesture(mAvatar, ovrAvatarHandGesture_Default);
	}
	
	if (mCurrentGestures[GHOculusUtil::RIGHTHAND])
	{
		setGesture(*mCurrentGestures[GHOculusUtil::RIGHTHAND]);
	}
	else
	{
		ovrAvatar_SetRightHandGesture(mAvatar, ovrAvatarHandGesture_Default);
	}
}

void GHOculusAvatarCustomGestureSetter::setGesture(const GHIdentifier& id)
{
	auto iter = mGestureMap.find(id);
	if (iter == mGestureMap.end()
		|| !iter->second)
	{
		GHDebugMessage::outputString("Error loading gesture with id %d (%s). Not in map", static_cast<GHIdentifier::IDType>(id), id.getString() ? "" : id.getString());
		return;
	}

	Gesture& gesture = *iter->second;

	setGesture(gesture);
}

void GHOculusAvatarCustomGestureSetter::setGesture(Gesture& gesture)
{
	assert(static_cast<unsigned int>(gesture.mComponentType) < 2);
	mCurrentGestures[gesture.mComponentType] = &gesture;

	if (gesture.mIsDefault)
	{
		switch (gesture.mComponentType)
		{
		case GHOculusUtil::LEFTHAND:
			ovrAvatar_SetLeftHandGesture(mAvatar, ovrAvatarHandGesture_Default);
			break;
		case GHOculusUtil::RIGHTHAND:
			ovrAvatar_SetRightHandGesture(mAvatar, ovrAvatarHandGesture_Default);
			break;
		}
		return;
	}

	//lazy load
	if (!gesture.mJoints.size())
	{
		GHOculusUtil::loadGesture(mAvatar, gesture.mComponentType, gesture.mFilename.getChars(), mXMLSerializer, gesture.mJoints);
	}

	switch (gesture.mComponentType)
	{
	case GHOculusUtil::LEFTHAND:
		ovrAvatar_SetLeftHandCustomGesture(mAvatar, (uint32_t)gesture.mJoints.size(), &gesture.mJoints.front());
		break;
	case GHOculusUtil::RIGHTHAND:
		ovrAvatar_SetRightHandCustomGesture(mAvatar, (uint32_t)gesture.mJoints.size(), &gesture.mJoints.front());
		break;
	}
}