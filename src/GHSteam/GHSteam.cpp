// Copyright 2010 Golden Hammer Software
#include "GHSteam.h"
#include "steam_api.h"
#include "GHUtils/GHControllerMgr.h"

GHSteam::GHSteam(GHControllerMgr& controllerMgr)
	: mAppID(0)
	, mControllerMgr(controllerMgr)
{
	mInitialized = SteamAPI_Init();
	if (mInitialized)
	{
		mAppID = SteamUtils()->GetAppID();
		mControllerMgr.add(this);
	}
}

GHSteam::~GHSteam(void)
{
	if (mInitialized)
	{
		mControllerMgr.remove(this);
	}
}

void GHSteam::update(void)
{
	SteamAPI_RunCallbacks();
}

void GHSteam::onActivate(void)
{

}

void GHSteam::onDeactivate(void)
{

}

void GHSteam::printErrorResult(const char* message, EResult result) const
{
	const char* resultStr = 0;

	switch (result)
	{
#define CASE(RESULT) case EResult::k_##RESULT: resultStr = #RESULT; break;
		CASE(EResultFail);
		CASE(EResultNoConnection);
		CASE(EResultNotLoggedOn);
#undef CASE
	default: resultStr = 0; break;
	}

	if (resultStr) {
		GHDebugMessage::outputString("%s: %s", message, resultStr);
	}
	else {
		GHDebugMessage::outputString("%s: result value %d", message, result);
	}
	
}