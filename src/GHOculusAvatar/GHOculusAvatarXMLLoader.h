#pragma once
#include "GHXMLObjLoader.h"
#include <cstdint>

class GHTransform;
class GHOculusAvatarPlatformPoserFactory;
class GHVBFactory;
class GHStringIdFactory;
class GHInputState;
class GHPropertyContainer;
class GHXMLSerializer;
class GHEventMgr;
class GHModel;
class GHResourceFactory;
class GHMaterialCallbackMgr;
class GHOculusAvatar;

class GHOculusAvatarXMLLoader : public GHXMLObjLoader
{
public:
	GHOculusAvatarXMLLoader(const char* appID,
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
							GHResourceFactory& resourceFactory);
	virtual ~GHOculusAvatarXMLLoader(void);
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;

private:
	GHModel* extractModel(GHPropertyContainer& extraData) const;
	void loadComponentOverrides(const GHXMLNode& node, GHOculusAvatar* ret) const;

private:
	uint64_t mUserID; //Todo: set up a way to create different avatars with different user ids (eg, for network play with avatars)
	const GHXMLObjFactory& mXMLObjFactory;
	const GHTransform& mHMDOrigin;
	GHOculusAvatarPlatformPoserFactory& mAvatarPoserFactory;
	GHVBFactory& mVBFactory;
	GHMaterialCallbackMgr& mMaterialCallbackMgr;
	const GHStringIdFactory& mIdFactory;
	const GHInputState& mInputState;
	const GHPropertyContainer& mAppProps;
	const GHXMLSerializer& mXMLSerializer;
	GHEventMgr& mSystemEventMgr;
	GHEventMgr& mGameEventMgr;
	GHResourceFactory& mResourceFactory;
	bool mAPIInitialized{ false };
};
