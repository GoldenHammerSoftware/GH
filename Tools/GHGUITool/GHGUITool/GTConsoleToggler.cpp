// Copyright 2010 Golden Hammer Software
#include "GTConsoleToggler.h"
#include "GHInputState.h"
#include "GHUtils/GHStateMachine.h"
#include "GHKeyDef.h"
#include "GTIdentifiers.h"

GTConsoleToggler::GTConsoleToggler(const GHInputState& inputState, GHStateMachine& stateMachine)
	: mInputState(inputState)
	, mStateMachine(stateMachine)
	, mInConsole(false)
{

}

void GTConsoleToggler::update(void)
{
	if (mInputState.checkKeyChange(0, GHKeyDef::KEY_ENTER, true))
	{
		if (mInConsole)
		{
			mStateMachine.setState(GTIdentifiers::S_MAIN);
			mInConsole = false;
		}
		else
		{
			mStateMachine.setState(GTIdentifiers::S_CONSOLE);
			mInConsole = true;
		}
	}
}

void GTConsoleToggler::onActivate(void)
{

}

void GTConsoleToggler::onDeactivate(void)
{

}