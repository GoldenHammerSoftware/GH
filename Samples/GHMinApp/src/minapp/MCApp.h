// Copyright 2010 Golden Hammer Software
#pragma once

#include "Base/GHApp.h"
#include "Base/GHAppShard.h"
#include "GHPlatform/GHTimeVal.h"

class GHSystemServices;
class GHRenderServices;
class GHGameServices;
class GHPhysicsServices;
class GHGUISystem;
class GHPhysicsSim;
class GHSceneRenderer;
class MCRenderer;

class MCApp : public GHApp
{
public:
	MCApp(GHSystemServices& systemServices,
		GHRenderServices& renderServices,
		GHGameServices& gameServices);
	~MCApp(void);

	virtual void runFrame(void);

private:
	void initBasic(GHSystemServices& systemServices,
		GHRenderServices& renderServices,
		GHGameServices& gameServices);
	void initFPSCounter(GHSystemServices& systemServices);
	void initGUI(GHSystemServices& systemServices);
	void initDebug(GHSystemServices& systemServices, GHRenderServices& renderServices);
	void initMCLoaders(GHSystemServices& systemServices, GHRenderServices& renderServices);

private:
	GHAppShard mAppShard;
	GHPhysicsServices* mPhysicsServices;
	GHGUISystem* mGUISystem;
	GHPhysicsSim* mPhysicsSim;

	GHTimeVal mRealTimeVal;
	GHTimeVal mSimTimeVal;
	GHSceneRenderer* mSceneRenderer;
	MCRenderer* mMCRenderer;
};
