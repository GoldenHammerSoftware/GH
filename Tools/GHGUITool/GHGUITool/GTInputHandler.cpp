// Copyright 2010 Golden Hammer Software
#include "GTInputHandler.h"
#include "GHInputState.h"
#include "GHUtils/GHMessageHandler.h"
#include "GTIdentifiers.h"
#include "GHUtils/GHMessage.h"
#include "GHKeyDef.h"
#include "GHMath/GHFloat.h"

GTInputHandler::GTInputHandler(const GHInputState& inputState, GHMessageHandler& messageHandler)
	: mInputState(inputState)
	, mMessageHandler(messageHandler)
{

}

void GTInputHandler::update(void)
{
	if (mInputState.checkKeyChange(0, 'n', true))
	{
		GHMessage message(GTIdentifiers::M_CREATEPANEL);
		mMessageHandler.handleMessage(message);
	}

	if (mInputState.checkKeyChange(0, 'c', true))
	{
		if (mInputState.checkKeyChange(0, GHKeyDef::KEY_CONTROL, true))
		{
			//todo: handle ctrl+c
		}
		else
		{
			GHMessage message(GTIdentifiers::M_CREATEPANEL);
			message.getPayload().setProperty(GTIdentifiers::MP_CREATEASCHILD, true);
			mMessageHandler.handleMessage(message);
		}
	}

	if (mInputState.checkKeyChange(0, GHKeyDef::KEY_BACKSPACE, true)
		|| mInputState.checkKeyChange(0, GHKeyDef::KEY_DELETE, true))
	{
		GHMessage message(GTIdentifiers::M_DELETEPANEL);
		mMessageHandler.handleMessage(message);
	}

	if (mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE1, true))
	{
		GHMessage message(GTIdentifiers::M_SELECTPANEL);
		message.getPayload().setProperty(GTIdentifiers::MP_SELECTPOINT, &mInputState.getPointerPosition(0));
		mMessageHandler.handleMessage(message);
		mLastMousePos = mInputState.getPointerPosition(0);
	}
	else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_MOUSE1, false))
	{
		GHMessage message(GTIdentifiers::M_MOVEEND);
		mMessageHandler.handleMessage(message);
	}
	else if (mInputState.getKeyState(0, GHKeyDef::KEY_MOUSE1))
	{
		const GHPoint2& currPos = mInputState.getPointerPosition(0);
		GHPoint2 delta = currPos;
		delta -= mLastMousePos;
		if (!GHFloat::isZero(delta))
		{
			GHMessage message(GTIdentifiers::M_MOVEPANEL);
			message.getPayload().setProperty(GTIdentifiers::MP_MOVEDELTA, &delta);
			mMessageHandler.handleMessage(message);
		}
		mLastMousePos = currPos;
	}

	if (mInputState.checkKeyChange(0, GHKeyDef::KEY_SHIFT, true))
	{
		mMessageHandler.handleMessage(GHMessage(GTIdentifiers::M_SHIFTPRESSED));
	}
	else if (mInputState.checkKeyChange(0, GHKeyDef::KEY_SHIFT, false))
	{
		mMessageHandler.handleMessage(GHMessage(GTIdentifiers::M_SHIFTRELEASED));
	}

	if (mInputState.checkKeyChange(0, 'z', true))
	{
		if (mInputState.getKeyState(0, GHKeyDef::KEY_CONTROL))
		{
			if (mInputState.getKeyState(0, GHKeyDef::KEY_SHIFT))
			{
				GHMessage message(GTIdentifiers::M_REDO);
				mMessageHandler.handleMessage(message);
			}
			else
			{
				GHMessage message(GTIdentifiers::M_UNDO);
				mMessageHandler.handleMessage(message);
			}
		}
	}

	if (mInputState.checkKeyChange(0, 's', true))
	{
		if (mInputState.getKeyState(0, GHKeyDef::KEY_CONTROL))
		{
			GHMessage message(GTIdentifiers::M_SAVE);
			mMessageHandler.handleMessage(message);
		}
	}

	if (mInputState.checkKeyChange(0, 'o', true))
	{
		if (mInputState.getKeyState(0, GHKeyDef::KEY_CONTROL))
		{
			GHMessage clearMsg(GTIdentifiers::M_CLEARALLDATA);
			mMessageHandler.handleMessage(clearMsg);
			GHMessage loadMsg(GTIdentifiers::M_LOAD);
			mMessageHandler.handleMessage(loadMsg);
		}
	}
}

void GTInputHandler::onActivate(void)
{

}

void GTInputHandler::onDeactivate(void)
{

}
