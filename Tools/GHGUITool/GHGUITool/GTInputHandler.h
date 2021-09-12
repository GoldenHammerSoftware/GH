// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHMath/GHPoint.h"

class GHInputState;
class GHMessageHandler;

class GTInputHandler : public GHController
{
public:
	GTInputHandler(const GHInputState& inputState, GHMessageHandler& messageHandler);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	const GHInputState& mInputState;
	GHMessageHandler& mMessageHandler;
	GHPoint2 mLastMousePos;
};
