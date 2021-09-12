// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHInputState;
class GHStateMachine;

class GTConsoleToggler : public GHController
{
public:
	GTConsoleToggler(const GHInputState& inputState, GHStateMachine& stateMachine);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	const GHInputState& mInputState;
	GHStateMachine& mStateMachine;
	bool mInConsole;
};
