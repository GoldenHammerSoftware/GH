// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHApp.h"
#include "GHControllerMgr.h"
#include "GHTimeVal.h"
#include "GHOwnershipList.h"
#include "GHDeletionHandle.h"
#include "GHCamera.h"
#include "GHSceneRenderer.h"
#include <vector>
#include "GHAppShard.h"

class GHRenderServices;
class GHSystemServices;
class GHPhysicsServices;
class GHGameServices;
class GHGUISystem;
class GHPhysicsSim;
class GHMessageQueue;
class CGGame;
class CGRayPicker;

class CGApp : public GHApp
{
public:
    CGApp(GHSystemServices& systemServices,
          GHRenderServices& renderServices,
          GHGameServices& gameServices,
          GHMessageQueue& messageQueue);
    ~CGApp(void);
    
	virtual void runFrame(void);
	virtual void osFrameUpdate(void);
    
private:
    void initRendering(GHRenderServices& renderServices);
    void initDebugCam(void);
    void initMenuStates(void);
    void initGameStates(void);

private:
    GHSystemServices& mSystemServices;
    GHRenderServices& mRenderServices;
    GHPhysicsServices* mPhysicsServices;
    GHAppShard mAppShard;
    GHGUISystem* mGUISystem;
    GHPhysicsSim* mPhysicsSim;
    GHOwnershipList<GHDeletionHandle> mOwnershipList;
    GHTimeVal mTimeVal;
    GHCamera mCamera;
    GHCamera mShadowCamera;
    GHSceneRenderer mSceneRenderer;
    CGGame* mGame;
    GHStateMachine mGameInputStateMachine;
    CGRayPicker* mRayPicker;
};
