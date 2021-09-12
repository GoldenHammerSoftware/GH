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
class GHRenderTarget;

class GHModel;
class GHEntity;

class GHTestApp : public GHApp
{
public:
    GHTestApp(GHSystemServices& systemServices,
              GHRenderServices& renderServices,
              GHGameServices& gameServices);
    ~GHTestApp(void);
    
	virtual void runFrame(void);
	virtual void osFrameUpdate(void);
    
private:
    void createScene(void);
    void initRendering(GHRenderServices& renderServices);
    void createGHAM(const char* srcModelName, const char* destModelName);
    void profileGHAM(const char* srcModelName, const char* destModelName);

private:
    GHSystemServices& mSystemServices;
    GHRenderServices& mRenderServices;
    GHPhysicsServices* mPhysicsServices;
    GHAppShard mAppShard;
    GHGUISystem* mGUISystem;
    GHPhysicsSim* mPhysicsSim;
    int mFrameId;
    
    GHOwnershipList<GHDeletionHandle> mOwnershipList;

    GHTimeVal mTimeVal;

    GHCamera mCamera;
    GHSceneRenderer mSceneRenderer;
    GHRenderTarget* mShadowTarget;
    
    GHEntity* mLevelEnt;
    GHEntity* mPlayerEnt;
};
