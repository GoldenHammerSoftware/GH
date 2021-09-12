// Copyright 2010 Golden Hammer Software
#include "GHRigidBodyTool.h"
#include "GHBulletPhysicsServices.h"
#include "GHMaterial.h"
#include "GHRenderServices.h"
#include "GHSystemServices.h"
#include "GHGameServices.h"
#include "GHRenderTargetFactory.h"
#include "GHDebugDraw.h"
#include "GHTimeCalculator.h"
#include "GHTimeUpdater.h"
#include "GHInputObjectMover.h"
#include "GHKeyDef.h"
#include "GHGUISystem.h"
#include "GHGUIMgr.h"
#include "GHPhysicsSim.h"
#include "GHFramerateMonitor.h"
#include "GHRenderPass.h"
#include "GHRenderTargetMaterialCallbackFactory.h"
#include "GHMaterialCallbackMgr.h"
#include "GHProfiler.h"
#include "GHRenderTarget.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "GHEntityHashes.h"
#include "GHRagdollToolXMLLoader.h"
#include "GHRagdollToolPhysicsPauser.h"
#include "GHRagdollToolBoneSelector.h"
#include "GHRenderDevice.h"
#include "GHRagdollGenerator.h"
#include "GHXMLNode.h"
#include "GHPi.h"
#include "GHRagdollToolProperties.h"
#include "GHControllerTransition.h"
#include "GHRagdollToolCommandStateToggler.h"
#include "GHRagdollToolCommandStringInputHandler.h"
#include "GHRagdollToolCommandHandler.h"
#include "GHRagdollToolCommandParser.h"
#include "GHRagdollToolOffsetAdjuster.h"
#include "GHRagdollToolSaveInputHandler.h"
#include "GHRagdollToolSavedRagdollXMLLoader.h"
#include "GHRagdollToolObjectPusher.h"
#include "GHRagdollToolPivotDisplayer.h"

#define ENUMHEADER "GHRagdollToolProperties.h"
#include "GHEnumInclude.h"
#define ENUMHEADER "GHRagdollToolCollisionLayers.h"
#include "GHEnumInclude.h"

//#define LOADFINISHEDRAGDOLLONLY

void GHRigidBodyTool::createScene(void)
{
    mActiveID = mSystemServices.mHashTable.generateHash("ES_ACTIVE");
    
    mLevelEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("testlevel.xml");
    if (mLevelEnt) {
        mLevelEnt->mStateMachine.setState(mActiveID);
    }
    
    spawnRagdollEnt();
    if (mRagdollEnt)
    {
        GHPropertyContainer extraData;
        extraData.setProperty(GHEntityHashes::P_ENTITY, mRagdollEnt);
        //Description
        //mRagdollGenerator = (GHRagdollGenerator*)mAppShard.mXMLObjFactory.load("snowboarderragdolldesc.xml", &extraData);
        //Saved ragdoll
        //mRagdollGenerator = (GHRagdollGenerator*)mAppShard.mXMLObjFactory.load("ragdolltooloutput.xml", &extraData);
        mRagdollGenerator = (GHRagdollGenerator*)mAppShard.mXMLObjFactory.load("snowboarderragdoll2.xml", &extraData);
    }
}

void GHRigidBodyTool::spawnRagdollEnt(void)
{
    mRagdollEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("snowboarderent.xml");
    if (mRagdollEnt) {
        
        GHModel* model = mRagdollEnt->mProperties.getProperty(GHEntityHashes::MODEL);
        if (model && model->getSkeleton()) {
            GHTransform spawnPos;
            spawnPos.becomeYRotation(GHPI/2);
            spawnPos.setTranslate(GHPoint3(0, 0, 2));
            model->getSkeleton()->getLocalTransform() = spawnPos;
        }
         mRagdollEnt->mStateMachine.setState(mActiveID);
    }
}

void GHRigidBodyTool::resetRagdoll(void)
{
    GHEntity* oldRagdollEnt = mRagdollEnt;
    spawnRagdollEnt();
#ifndef LOADFINISHEDRAGDOLLONLY
    mRagdollGenerator->reset(mRagdollEnt);
#endif
    delete oldRagdollEnt;
}

GHRigidBodyTool::GHRigidBodyTool(GHSystemServices& systemServices,
                     GHRenderServices& renderServices,
                     GHGameServices& gameServices)
: mSystemServices(systemServices)
, mRenderServices(renderServices)
, mPhysicsServices(0)
, mPhysicsSim(0)
, mAppShard(systemServices)
, mSceneRenderer(*renderServices.getDevice(), mAppShard.mProps)
, mShadowTarget(renderServices.getRenderTargetFactory()->createRenderTarget(128,128))
, mLevelEnt(0)
, mRagdollEnt(0)
, mCurrentlySelectedBone(0)
, mCurrentlySelectedPhysicsObject(0)
, mFrameId(0)
{
    mSystemServices.initAppShard(mAppShard);
    mRenderServices.initAppShard(mAppShard);
    mPhysicsServices = new GHBulletPhysicsServices(mSystemServices, GHPoint3(0, -9.8, 0));
    mPhysicsSim = mPhysicsServices->createPhysicsSim(mTimeVal);
    mPhysicsServices->initAppShard(mAppShard, *mPhysicsSim, mSystemServices.mEnumStore);
    
    GHMaterial* debugMaterial = (GHMaterial*)mAppShard.mXMLObjFactory.load("debugmat.xml");
    GHDebugDraw* debugDraw = new GHDebugDraw(*mRenderServices.getVBFactory(), debugMaterial);
    GHDebugDraw::setSingleton(debugDraw);
    mSceneRenderer.getRenderGroup().addRenderable(GHDebugDraw::getSingleton(), 0);
    
    GHRagdollToolCollisionLayers::generateEnum(mSystemServices.mEnumStore);
    
    gameServices.initAppShard(mAppShard, mTimeVal, mSceneRenderer.getRenderGroup(), *mPhysicsSim);
    
    GHRagdollToolProperties::generateIdentifiers(mSystemServices.mHashTable);
    
//    mAppShard.mXMLObjFactory.addLoader(new GHRigidBodyToolRagdollInputXMLLoader(*mSystemServices.mInputState, mAppShard.mControllerMgr), 1, "testappragdollinput");
    
    srand (mSystemServices.mTimeCalc->calcTime()*INT_MAX);
    
    GHController* timeUpdater = new GHTimeUpdater(*mSystemServices.mTimeCalc, mTimeVal);
    mAppShard.mControllerMgr.add(timeUpdater);
    mOwnershipList.push_back(new GHTypedDeletionHandle<GHController>(timeUpdater));
    
    
    GHRagdollToolXMLLoader* ragdollGeneratorLoader =
    new GHRagdollToolXMLLoader(mAppShard.mXMLObjFactory,
                               mSystemServices.mEnumStore,
                               mSystemServices.mHashTable,
                               mAppShard.mControllerMgr, *mPhysicsSim,
                               mSystemServices.mXMLSerializer);
    mAppShard.mXMLObjFactory.addLoader(ragdollGeneratorLoader, 1, "ragdollInput");
  
#ifndef LOADFINISHEDRAGDOLLONLY
    //This overrides the normal ragdoll loading code, so we turn it off if we're not building
    // the tool nodes around the ragdoll.
    GHRagdollToolSavedRagdollXMLLoader* savedRagdollLoader =
    new GHRagdollToolSavedRagdollXMLLoader(mAppShard.mXMLObjFactory,
                                           mSystemServices.mEnumStore,
                                           mSystemServices.mHashTable,
                                           mAppShard.mControllerMgr,
                                           *mPhysicsSim,
                                           mSystemServices.mXMLSerializer);
    mAppShard.mXMLObjFactory.addLoader(savedRagdollLoader, 1, "ragdoll");
#endif
    
    initRendering(renderServices);
    
    GHInputObjectMover* camMover = new GHInputObjectMover(mCamera.getTransform(),
                                                          *mSystemServices.mInputState, mTimeVal);
    camMover->addCommand(GHKeyDef::KEY_LEFTARROW, GHInputObjectMover::OM_LEFT);
    camMover->addCommand(GHKeyDef::KEY_RIGHTARROW, GHInputObjectMover::OM_RIGHT);
    camMover->addCommand(GHKeyDef::KEY_UPARROW, GHInputObjectMover::OM_FORWARD);
    camMover->addCommand(GHKeyDef::KEY_DOWNARROW, GHInputObjectMover::OM_BACK);
    /*
    camMover->addCommand('a', GHInputObjectMover::OM_LEFT);
    camMover->addCommand('d', GHInputObjectMover::OM_RIGHT);
    camMover->addCommand('w', GHInputObjectMover::OM_FORWARD);
    camMover->addCommand('s', GHInputObjectMover::OM_BACK);
    */
    camMover->addCommand(' ', GHInputObjectMover::OM_UP);
    camMover->addCommand('c', GHInputObjectMover::OM_DOWN);
    camMover->addCommand(GHKeyDef::KEY_MOUSE1, GHInputObjectMover::OM_ROTATE);
    camMover->addCommand(GHKeyDef::KEY_SHIFT, GHInputObjectMover::OM_SPRINT);
    
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     camMover));
    
    mGUISystem = new GHGUISystem(mSystemServices, renderServices,
                                 mSceneRenderer.getRenderGroup(), mAppShard, mTimeVal);
    mAppShard.mControllerMgr.add(mGUISystem->mGUIMgr);
    
    mPhysicsSim->pause();
    mAppShard.mControllerMgr.add(mPhysicsSim);
    
    GHRagdollToolPhysicsPauser* physicsPauser = new GHRagdollToolPhysicsPauser(*mSystemServices.mInputState, mAppShard.mControllerMgr, *mPhysicsSim, *this, mAppShard.mProps);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     physicsPauser));
    
    GHRagdollToolBoneSelector* boneSelector = new GHRagdollToolBoneSelector(*mSystemServices.mInputState, mRenderServices.getDevice()->getViewInfo(), *mPhysicsSim, mAppShard.mProps, *mRagdollGenerator, mCurrentlySelectedBone, mCurrentlySelectedPhysicsObject);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     boneSelector));
    
    GHRagdollToolOffsetAdjuster* offsetAdjuster = new GHRagdollToolOffsetAdjuster(*mSystemServices.mInputState, mCurrentlySelectedBone, mAppShard.mProps);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     offsetAdjuster));
    
    GHRagdollToolSaveInputHandler* saveInputHandler = new GHRagdollToolSaveInputHandler(mSystemServices.mXMLSerializer, *mSystemServices.mInputState, *mRagdollGenerator, mAppShard.mProps);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     saveInputHandler));
    
    GHRagdollToolObjectPusher* objectPusher = new GHRagdollToolObjectPusher(*mSystemServices.mInputState, mAppShard.mProps, mCurrentlySelectedPhysicsObject);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     objectPusher));
    
    GHRagdollToolPivotDisplayer* pivotDisplayer = new GHRagdollToolPivotDisplayer(*mSystemServices.mInputState, mCurrentlySelectedBone);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::MAININPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     pivotDisplayer));
    
    GHRagdollToolCommandStateToggler* commandStateToggler = new GHRagdollToolCommandStateToggler(mAppShard.mStateMachine, *mSystemServices.mInputState);
    mAppShard.mControllerMgr.add(commandStateToggler);
    
    GHRagdollToolCommandHandler* commandHandler = new GHRagdollToolCommandHandler(*mRagdollGenerator, mCurrentlySelectedBone, mSystemServices.mHashTable);
    mOwnershipList.push_back(new GHTypedDeletionHandle<GHRagdollToolCommandHandler>(commandHandler));
    
    GHRagdollToolCommandParser* commandParser = new GHRagdollToolCommandParser(*commandHandler, mSystemServices.mEnumStore, mAppShard.mProps);
    mOwnershipList.push_back(new GHTypedDeletionHandle<GHRagdollToolCommandParser>(commandParser));
    
    GHRagdollToolCommandStringInputHandler* commandStringInputHandler = new GHRagdollToolCommandStringInputHandler(*mSystemServices.mInputState,
                                                                                                        mAppShard.mProps,
                                                                                                        mTimeVal,
                                                                                                        *commandParser);
    mAppShard.mStateMachine.addTransition(GHRagdollToolProperties::COMMANDINPUTSTATE,
                                          new GHControllerTransition(mAppShard.mControllerMgr,
                                                                     commandStringInputHandler));
    
    GHIdentifier fpsProp = mSystemServices.mHashTable.generateHash("FPS");
    GHIdentifier msProp = mSystemServices.mHashTable.generateHash("MS");
    GHFramerateMonitor* fpsMon = new GHFramerateMonitor(mTimeVal, mAppShard.mProps,
                                                        fpsProp, msProp);
    mAppShard.mControllerMgr.add(fpsMon);
    
    GHGUIWidgetResource* testGUI = (GHGUIWidgetResource*)mAppShard.mXMLObjFactory.load("gui.xml");
    if (testGUI) {
        mGUISystem->mGUIMgr->pushWidget(testGUI, GHGUITransitionDesc(),0);
    }
    
    mAppShard.mStateMachine.setState(GHRagdollToolProperties::MAININPUTSTATE);
}

GHRigidBodyTool::~GHRigidBodyTool(void)
{
    mSceneRenderer.getRenderGroup().removeRenderable(GHDebugDraw::getSingleton(), 0);
    
    delete mRagdollGenerator;
    delete mRagdollEnt;
    delete mLevelEnt;
    delete mPhysicsSim;
    delete mShadowTarget;
    delete mGUISystem;
    delete mPhysicsServices;
    
    mAppShard.mStateMachine.setState(-1);
    
    GHDebugDraw::setSingleton(0);
}

void GHRigidBodyTool::initRendering(GHRenderServices& renderServices)
{
    GHPoint4 clearColor(1,0,0,0);
    GHIdentifier shadowId = mSystemServices.mHashTable.generateHash("shadow");
    GHRenderPass* shadowPass = new GHRenderPass(mCamera, renderServices.getScreenInfo(),
                                                mShadowTarget, *mRenderServices.getDevice(),
                                                true, &clearColor, shadowId);
    mSceneRenderer.addRenderPass(shadowPass);
    GHMaterialCallbackFactory* shadowCallback =
    new GHRenderTargetMaterialCallbackFactory("ShadowTexture",
                                              GHMaterialCallbackType::CT_PERFRAME,
                                              *mShadowTarget);
    renderServices.getMaterialCallbackMgr()->addFactory(shadowCallback);
    
    clearColor.setCoords(0,0,1,0);
    GHRenderPass* renderPass = new GHRenderPass(mCamera, renderServices.getScreenInfo(), 0,
                                                *mRenderServices.getDevice(), true, &clearColor,
                                                GHRenderPass::sMainPassId);
    mSceneRenderer.addRenderPass(renderPass);
    createScene();
    mAppShard.mControllerMgr.add(&mSceneRenderer);
    
    mCamera.getTransform().setTranslate(GHPoint3(0,2,0));
    mCamera.setNearClip(0.01f);
    mCamera.setFarClip(100.0f);
}

void GHRigidBodyTool::runFrame(void)
{
    if (mFrameId == 10) {
        GHPROFILESTART
    }
    else if (mFrameId == 100) {
        GHPROFILESTOP
        GHPROFILEOUTPUT
    }
    mFrameId++;
    
    mAppShard.mControllerMgr.update();
}

void GHRigidBodyTool::osFrameUpdate(void)
{
}