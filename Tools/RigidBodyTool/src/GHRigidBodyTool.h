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
#include "GHIdentifier.h"

class GHRenderServices;
class GHSystemServices;
class GHPhysicsServices;
class GHGameServices;
class GHGUISystem;
class GHPhysicsSim;
class GHRenderTarget;
class GHRagdollToolNode;
class GHRagdollGenerator;
class GHPhysicsObject;

class GHModel;
class GHEntity;

class GHRigidBodyTool : public GHApp
{
public:
    GHRigidBodyTool(GHSystemServices& systemServices,
                    GHRenderServices& renderServices,
                    GHGameServices& gameServices);
    ~GHRigidBodyTool(void);

    virtual void runFrame(void);
	virtual void osFrameUpdate(void);
    
    void resetRagdoll(void);
    
private:
    void initRendering(GHRenderServices& renderServices);
    void createScene(void);
    void spawnRagdollEnt(void);
    
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
    GHEntity* mRagdollEnt;
    GHRagdollToolNode* mCurrentlySelectedBone;
    GHPhysicsObject* mCurrentlySelectedPhysicsObject;
    GHRagdollGenerator* mRagdollGenerator;
    GHIdentifier mActiveID;
};
