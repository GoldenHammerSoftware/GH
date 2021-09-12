// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHApp.h"
#include "GHAppShard.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHCamera.h"
#include "GHGUIWidget.h"

class GHSystemServices;
class GHRenderServices;
class GHGameServices;
class GHMessageQueue;
class GHGUISystem;
class GHSceneRenderer;

class GHGUIToolApp : public GHApp
{
public:
	GHGUIToolApp(GHSystemServices& systemServices,
				GHRenderServices& renderServices,
				GHGameServices& gameServices,
				GHMessageQueue& appMessageQueue);
	~GHGUIToolApp(void);

	virtual void runFrame(void);

private:
	void initBasic(GHSystemServices& systemServices, GHRenderServices& renderServices, GHGameServices& gameServices);
	void initGUI(GHSystemServices& systemServices);
	void initRendering(GHSystemServices& systemServices, GHRenderServices& renderServices);
	void initTool(GHSystemServices& systemServices, GHRenderServices& renderServices, GHMessageHandler& appMessageQueue);

	GHAppShard mAppShard;
	GHGUISystem* mGUISystem;
	GHGUIWidgetResource* mTopLevelGUI;
	GHGUIWidgetResource* mCurrentlySelectedWidget;
	GHTimeVal mTimeVal;
	GHCamera mCamera;
	GHSceneRenderer* mSceneRenderer;

	GHMessageQueue& mAppMessageQueue;
};
