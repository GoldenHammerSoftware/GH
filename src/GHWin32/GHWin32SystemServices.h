// Copyright Golden Hammer Software
#pragma once

#include "Base/GHSystemServices.h"

class GHWin32Window;
class GHControllerMgr;
class GHWin32SoundFinder;

class GHWin32SystemServices : public GHSystemServices
{
public:
	GHWin32SystemServices(GHWin32Window& window, GHControllerMgr& controllerManager);

	virtual void initAppShard(GHAppShard& appShard);

private:
	GHWin32SoundFinder* mSoundFinder;
};
