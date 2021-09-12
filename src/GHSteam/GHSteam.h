// Copyright 2010 Golden Hammer Software
#pragma once
#include <cstdint>
#include "GHUtils/GHController.h"
#include "steam_api.h"

class GHControllerMgr;

class GHSteam : public GHController
{
public:
	GHSteam(GHControllerMgr& controllerMgr);
	virtual ~GHSteam(void);

	bool isAvailable(void) const { return mInitialized; }
	int64_t getAppID(void) const { return mAppID; }
	void printErrorResult(const char* message, EResult result) const;

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	GHControllerMgr& mControllerMgr;
	bool mInitialized;
	int64_t mAppID;
};