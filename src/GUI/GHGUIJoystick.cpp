// Copyright Golden Hammer Software
#include "GHGUIJoystick.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHInputClaim.h"
#include "GHInputState.h"
#include "GHKeyDef.h"

GHGUIJoystick::GHGUIJoystick(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker,
	GHPropertyContainer& props, const GHIdentifier& activeProp,
	const GHIdentifier& xProp, const GHIdentifier& yProp, 
	float maxDiff, const std::vector<int>& allowedPointers,
	GHInputState& inputState, GHInputClaim& pointerClaim, GHControllerMgr& controllerMgr,
	GHGUIWidgetResource* joybg, GHGUIWidgetResource* joyfg)
: GHGUIPanel(renderer, rectMaker)
, mProps(props)
, mActiveProp(activeProp)
, mXProp(xProp)
, mYProp(yProp)
, mInputHandler(*this, inputState, pointerClaim, mScreenPos, maxDiff, allowedPointers)
, mControllerMgr(controllerMgr)
, mJoyBG(joybg)
, mJoyFG(joyfg)
{
	if (mJoyBG) mJoyBG->acquire();
	if (mJoyFG) mJoyFG->acquire();
	if (mJoyBG && mJoyFG) {
		mJoyBG->get()->addChild(mJoyFG);
	}
}

GHGUIJoystick::~GHGUIJoystick(void)
{
	if (mJoyBG) mJoyBG->release();
	if (mJoyFG) mJoyFG->release();
}

void GHGUIJoystick::onFinishTransitionIn(void)
{
	mControllerMgr.add(&mInputHandler);
}

void GHGUIJoystick::onEnterTransitionOut(void)
{
	mControllerMgr.remove(&mInputHandler);
}

void GHGUIJoystick::handleJoystickActivated(const GHPoint2& joyCenter)
{
	mProps.setProperty(mActiveProp, 1);

	GHPoint2 bgPos = convertScreenToLocal(joyCenter);
	if (mJoyBG)
	{
		GHGUIPosDesc posDesc = mJoyBG->get()->getPosDesc();
		posDesc.mAlign = bgPos;
		mJoyBG->get()->setPosDesc(posDesc);
		mJoyBG->get()->updatePosition();
		this->addChild(mJoyBG);
	}
	if (mJoyFG) {
		GHGUIPosDesc posDesc = mJoyFG->get()->getPosDesc();
		posDesc.mAlign = GHPoint2(0.5,0.5);
		mJoyFG->get()->setPosDesc(posDesc);
		mJoyFG->get()->updatePosition();
	}
}

void GHGUIJoystick::handleJoystickDeactivated(void)
{
	mProps.setProperty(mActiveProp, 0);
	mProps.setProperty(mXProp, 0);
	mProps.setProperty(mYProp, 0);

	if (mJoyBG) {
		this->removeChild(mJoyBG);
	}
}

void GHGUIJoystick::handleJoystickPosition(const GHPoint2& pos)
{
	mProps.setProperty(mXProp, pos[0]);
	mProps.setProperty(mYProp, pos[1]);

	if (mJoyFG) 
	{
		GHPoint2 fgPos(pos);
		float posLen = fgPos.normalize();
		if (posLen > 1.0f) posLen = 1.0f;
		fgPos *= posLen;
		fgPos *= 0.5f;
		fgPos += 0.5f;

		GHGUIPosDesc posDesc = mJoyFG->get()->getPosDesc();
		posDesc.mAlign = fgPos;
		mJoyFG->get()->setPosDesc(posDesc);
		mJoyFG->get()->updatePosition();
	}
}

GHPoint2 GHGUIJoystick::convertScreenToLocal(const GHPoint2& pos)
{
	GHPoint2 offset(pos);
	offset -= mScreenPos.mMin;
	GHPoint2 size(mScreenPos.mMax);
	size -= mScreenPos.mMin;
	offset[0] /= size[0];
	offset[1] /= size[1];
	return offset;
}

GHGUIJoystick::JoyInputHandler::JoyInputHandler(GHGUIJoystick& parent, GHInputState& inputState, 
	GHInputClaim& pointerClaim, const GHRect<float, 2>& screenPos, 
	float maxDiff, const std::vector<int>& allowedPointers)
: mParent(parent)
, mInputState(inputState)
, mPointerClaim(pointerClaim)
, mScreenPos(screenPos)
, mActivePointer(-1)
, mMaxDiff(maxDiff)
, mAllowedPointers(allowedPointers)
{
}

void GHGUIJoystick::JoyInputHandler::update(void)
{
	handlePointerClaim();

	if (mActivePointer != -1)
	{
		GHPoint2 pointerPos = mInputState.getPointerPosition(mActivePointer);
		pointerPos -= mJoyCenter;
		pointerPos /= mMaxDiff;
		pointerPos.clamp(GHPoint2(-1, -1), GHPoint2(1, 1));
		mParent.handleJoystickPosition(pointerPos);
	}
}

void GHGUIJoystick::JoyInputHandler::handlePointerClaim(void)
{
	// if we don't have an active pointer, then look for activates over our region.
	const GHInputStructs::InputEventList& events = mInputState.getInputEvents();
	for (size_t i = 0; i < events.size(); ++i)
	{
		if (events[i].mType == GHInputStructs::IET_KEYCHANGE || events[i].mType == GHInputStructs::IET_GAMEPADKEYCHANGE)
		{
			if (events[i].mVal[0] == GHKeyDef::KEY_MOUSE1)
			{
				if (!events[i].mVal[1])
				{
					tryDeactivateJoystick(events[i].mId);
				}
				else {
					tryActivateJoystick(0);
				}
			}
		}
		else if (events[i].mType == GHInputStructs::IET_POINTERACTIVATE)
		{
			tryActivateJoystick(events[i].mId);
		}
		else if (events[i].mType == GHInputStructs::IET_POINTERDEACTIVE)
		{
			tryDeactivateJoystick(events[i].mId);
		}
	}
}

bool GHGUIJoystick::JoyInputHandler::checkPointerAllowed(int pointerId)
{
	if (!mAllowedPointers.size()) {
		return true;
	}
	for (size_t i = 0; i < mAllowedPointers.size(); ++i)
	{
		if (mAllowedPointers[i] == pointerId) {
			return true;
		}
	}
	return false;
}

void GHGUIJoystick::JoyInputHandler::tryActivateJoystick(int pointerId)
{
	if (mActivePointer == -1)
	{
		if (checkPointerAllowed(pointerId)) {
			const GHPoint2& pointerPos = mInputState.getPointerPosition(pointerId);
			if (mScreenPos.containsPoint(pointerPos)) {
				if (mPointerClaim.claimPointer(pointerId, this))
				{
					mActivePointer = pointerId;
					mJoyCenter = pointerPos;
					mParent.handleJoystickActivated(mJoyCenter);
				}
			}
		}
	}
}

void GHGUIJoystick::JoyInputHandler::tryDeactivateJoystick(int pointerId)
{
	if (pointerId == mActivePointer)
	{
		mPointerClaim.releasePointer(mActivePointer, this);
		mActivePointer = -1;
		mParent.handleJoystickDeactivated();
	}
}

void GHGUIJoystick::JoyInputHandler::onActivate(void)
{
	mActivePointer = -1;
}

void GHGUIJoystick::JoyInputHandler::onDeactivate(void)
{
	if (mActivePointer != -1)
	{
		mPointerClaim.releasePointer(mActivePointer, this);
		mActivePointer = -1;
	}
}
