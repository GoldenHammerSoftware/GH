#pragma once
#include "GHXMLObjLoader.h"

class GHTimeVal;
class GHTransform;
class GHPropertyContainer;
class GHStringIdFactory;
class GHInputState;
class GHEventMgr;

//<vrRecenterController vrCenterId="GP_VRCENTEROFFSET" vrCenterDefaultId="GP_VRCENTERDEFAULT" optionsSaveMessageId="M_SAVEOPTIONS"/>
class GHHMDRecenterControllerXMLLoader : public GHXMLObjLoader
{
public:

	GHHMDRecenterControllerXMLLoader(const GHTimeVal& timeVal, GHTransform& hmdOrigin, GHPropertyContainer& globalProps,
									const GHStringIdFactory& hashtable,
									const GHInputState& inputState, 
									GHEventMgr& eventMgr,
									unsigned int gamepadIndex);
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHTimeVal& mTimeVal;
	GHTransform& mHMDOrigin;
	GHPropertyContainer& mGobalProps;
	const GHInputState& mInputState;
	const GHStringIdFactory& mIdFactory;
	GHEventMgr& mEventMgr;
	unsigned int mGamepadIndex;
};
