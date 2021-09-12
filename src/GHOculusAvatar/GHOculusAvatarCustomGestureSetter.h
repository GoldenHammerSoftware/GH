#pragma once

#include <vector>
#include <map>
#include "OVR_Avatar.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHIdentifier.h"
#include "GHString/GHString.h"
#include "GHOculusAvatarUtil.h"
#include "GHRenderDeviceReinitListener.h"

class GHEventMgr;
class GHXMLSerializer;
class GHStringIdFactory;

//M_SETHANDGESTURE

class GHOculusAvatarCustomGestureSetter : public GHMessageHandler
{
public:
	GHOculusAvatarCustomGestureSetter(ovrAvatar*& avatar, 
									  GHEventMgr& eventMgr, 
									  const GHXMLSerializer& xmlSerializer, 
									  const GHStringIdFactory& hashTable,
									  const char* configFilename);
	~GHOculusAvatarCustomGestureSetter(void);

	virtual void handleMessage(const GHMessage& message);

	void onDeviceReinit(void);

private:
	void loadGestureMap(const char* configFilename);
	void setGesture(const GHIdentifier& id);
	void handleSetHandGesture(const GHMessage& message);
	void handleSetCapabilities(const GHMessage& message);
	void handleResetCurrentHandGesture(void);

private:
	struct Gesture
	{
		GHString mFilename;
		std::vector<ovrAvatarTransform> mJoints;
		GHOculusUtil::AvatarComponentType mComponentType;
		bool mIsDefault;
	};
	void setGesture(Gesture& gesture);

private:
	ovrAvatar*& mAvatar;
	GHEventMgr& mEventMgr;
	const GHXMLSerializer& mXMLSerializer;
	const GHStringIdFactory& mIdFactory;
	std::map<GHIdentifier, Gesture*> mGestureMap;
	Gesture* mCurrentGestures[2]{ 0, 0 };
	GHRenderDeviceReinitListener<GHOculusAvatarCustomGestureSetter> mDeviceReinitListener;
};
