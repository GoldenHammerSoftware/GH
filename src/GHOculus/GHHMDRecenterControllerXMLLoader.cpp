#include "GHHMDRecenterControllerXMLLoader.h"
#include "GHHMDRecenterController.h"
#include "GHXMLNode.h"

GHHMDRecenterControllerXMLLoader::GHHMDRecenterControllerXMLLoader(const GHTimeVal& timeVal, GHTransform& hmdOrigin, GHPropertyContainer& globalProps,
																const GHStringIdFactory& hashtable,
																const GHInputState& inputState, 
																GHEventMgr& eventMgr,
																unsigned int gamepadIndex)
	: mTimeVal(timeVal)
	, mHMDOrigin(hmdOrigin)
	, mGobalProps(globalProps)
	, mInputState(inputState)
	, mIdFactory(hashtable)
	, mEventMgr(eventMgr)
	, mGamepadIndex(gamepadIndex)
{
}

void* GHHMDRecenterControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier vrCenterId, vrCenterDefaultId, optionsSaveMessageId;
	node.readAttrIdentifier("vrCenterId", vrCenterId, mIdFactory);
	node.readAttrIdentifier("vrCenterDefaultId", vrCenterDefaultId, mIdFactory);
	node.readAttrIdentifier("optionsSaveMessageId", optionsSaveMessageId, mIdFactory);
	return new GHHMDRecenterController(mTimeVal, mGobalProps, vrCenterId, vrCenterDefaultId, optionsSaveMessageId, mHMDOrigin, mInputState, mEventMgr, mGamepadIndex);
}