// Copyright 2010 Golden Hammer Software
#include "GHTestApp.h"
#include "GHDebugMessage.h"
#include "GHEventMgr.h"
#include "GHTimeUpdater.h"
#include "GHInputState.h"
#include "GHRenderServices.h"
#include "GHRenderDevice.h"
#include "GHSystemServices.h"
#include "GHGameServices.h"
#include "GHInputObjectMover.h"
#include "GHGUISystem.h"
#include "GHGUIMgr.h"
#include "GHEntity.h"
#include "GHEntityHashes.h"
#include "GHTimeCalculator.h"
#include "GHMaterialCallbackType.h"
#include "GHTestAppRagdollInputXMLLoader.h"

#include "GHFloat.h"
#include "GHPoint.h"

#include "GHRenderPass.h"
#include "GHRenderTargetFactory.h"
#include "GHRenderTarget.h"
#include "GHModel.h"
#include "GHGeometryRenderable.h"
#include "GHRenderTargetMaterialCallbackFactory.h"
#include "GHMaterialCallbackMgr.h"
#include "GHBulletPhysicsServices.h"
#include "GHPhysicsSim.h"
#include "GHFramerateMonitor.h"
#include "GHKeyDef.h"
#include "GHDebugDraw.h"

// part of gham test hack
#include "GHRefCountedDeletionHandle.h"
#include "GHAMSerializer.h"
#include "GHGeometryComparison.h"
#include "GHProfiler.h"

GHTestApp::GHTestApp(GHSystemServices& systemServices,
                     GHRenderServices& renderServices,
                     GHGameServices& gameServices)
: mSystemServices(systemServices)
, mRenderServices(renderServices)
, mPhysicsServices(0)
, mPhysicsSim(0)
, mAppShard(systemServices)
, mSceneRenderer(*renderServices.getDevice())
, mShadowTarget(renderServices.getRenderTargetFactory()->createRenderTarget(128,128))
, mLevelEnt(0)
, mPlayerEnt(0)
, mFrameId(0)
{
    mSystemServices.initAppShard(mAppShard);
    mRenderServices.initAppShard(mAppShard);
    mPhysicsServices = new GHBulletPhysicsServices(mTimeVal, mSystemServices, GHPoint3(0, -9.8, 0));
    mPhysicsSim = mPhysicsServices->createPhysicsSim();
    mPhysicsServices->initAppShard(mAppShard, *mPhysicsSim, mSystemServices.mEnumStore);
    
    GHMaterial* debugMaterial = (GHMaterial*)mAppShard.mXMLObjFactory.load("debugmat.xml");
    GHDebugDraw* debugDraw = new GHDebugDraw(*mRenderServices.getVBFactory(), debugMaterial);
    GHDebugDraw::setSingleton(debugDraw);
    mSceneRenderer.getRenderGroup().addRenderable(GHDebugDraw::getSingleton());
    
    gameServices.initAppShard(mAppShard, mTimeVal, mSceneRenderer.getRenderGroup(), *mPhysicsSim);
    
    mAppShard.mXMLObjFactory.addLoader(new GHTestAppRagdollInputXMLLoader(*mSystemServices.mInputState, mAppShard.mControllerMgr), 1, "testappragdollinput");
    
    srand (mSystemServices.mTimeCalc->calcTime()*INT_MAX);
    
    GHController* timeUpdater = new GHTimeUpdater(*mSystemServices.mTimeCalc, mTimeVal);
    mAppShard.mControllerMgr.add(timeUpdater);
    mOwnershipList.push_back(new GHTypedDeletionHandle<GHController>(timeUpdater));
    
    initRendering(renderServices);

    GHInputObjectMover* camMover = new GHInputObjectMover(mCamera.getTransform(), 
                                                          *mSystemServices.mInputState, mTimeVal);
    camMover->addCommand(GHKeyDef::KEY_LEFTARROW, GHInputObjectMover::OM_LEFT);
    camMover->addCommand(GHKeyDef::KEY_RIGHTARROW, GHInputObjectMover::OM_RIGHT);
    camMover->addCommand(GHKeyDef::KEY_UPARROW, GHInputObjectMover::OM_FORWARD);
    camMover->addCommand(GHKeyDef::KEY_DOWNARROW, GHInputObjectMover::OM_BACK);
    camMover->addCommand('a', GHInputObjectMover::OM_LEFT);
    camMover->addCommand('d', GHInputObjectMover::OM_RIGHT);
    camMover->addCommand('w', GHInputObjectMover::OM_FORWARD);
    camMover->addCommand('s', GHInputObjectMover::OM_BACK);
    camMover->addCommand(' ', GHInputObjectMover::OM_UP);
    camMover->addCommand('c', GHInputObjectMover::OM_DOWN);
    camMover->addCommand(GHKeyDef::KEY_MOUSE1, GHInputObjectMover::OM_ROTATE);
    camMover->addCommand(GHKeyDef::KEY_SHIFT, GHInputObjectMover::OM_SPRINT);
    mAppShard.mControllerMgr.add(camMover);
    mOwnershipList.push_back(new GHTypedDeletionHandle<GHInputObjectMover>(camMover));
    
    mGUISystem = new GHGUISystem(mSystemServices, renderServices, 
                                 mSceneRenderer.getRenderGroup(), mAppShard, mTimeVal);
    mAppShard.mControllerMgr.add(mGUISystem->mGUIMgr);
    
    mAppShard.mControllerMgr.add(mPhysicsSim);
    
    GHIdentifier fpsProp = mSystemServices.mHashTable.generateHash("FPS");
    GHIdentifier msProp = mSystemServices.mHashTable.generateHash("MS");
    GHFramerateMonitor* fpsMon = new GHFramerateMonitor(mTimeVal, mAppShard.mProps,
                                                        fpsProp, msProp);
    mAppShard.mControllerMgr.add(fpsMon);
    
    GHGUIWidgetResource* testGUI = (GHGUIWidgetResource*)mAppShard.mXMLObjFactory.load("testGUI.xml");
    if (testGUI) {
        mGUISystem->mGUIMgr->pushWidget(testGUI, GHGUITransitionDesc(),0);
    }
}

void GHTestApp::initRendering(GHRenderServices& renderServices)
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

GHTestApp::~GHTestApp(void)
{
    mSceneRenderer.getRenderGroup().removeRenderable(GHDebugDraw::getSingleton());
    
    delete mLevelEnt;
    delete mPlayerEnt;
    delete mPhysicsSim;
    delete mShadowTarget;
    delete mGUISystem;
    delete mPhysicsServices;
    
    GHDebugDraw::setSingleton(0);
}

void GHTestApp::runFrame(void)
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

void GHTestApp::osFrameUpdate(void)
{
}

void GHTestApp::createScene(void)
{
    GHIdentifier activeId = mSystemServices.mHashTable.generateHash("ES_ACTIVE");

    //createGHAM("cubemodel.xml", "dudeskelgham");
    /*
    //    createGHAM("cubemodel.xml", "cubegham");
        createGHAM("kyalegsmodel.xml", "kyalegsmodelgham");
        createGHAM("kyatorsomodel.xml", "kyatorsomodelgham");
        createGHAM("kyaheadmodel.xml", "kyaheadmodelgham");
     */
    
    mLevelEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("testlevel.xml");
    if (mLevelEnt) {
        mLevelEnt->mStateMachine.setState(activeId);
    }
    
    mPlayerEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("cubeent.xml");
    if (mPlayerEnt) {
        GHModel* playerModel = mPlayerEnt->mProperties.getProperty(GHEntityHashes::MODEL);
        if (playerModel)
        {
            playerModel->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(0, 0, 5));

            GHInputObjectMover* camMover = new GHInputObjectMover(playerModel->getSkeleton()->getLocalTransform(), 
                                                                *mSystemServices.mInputState, mTimeVal);
            camMover->addCommand('4', GHInputObjectMover::OM_LEFT);
            camMover->addCommand('6', GHInputObjectMover::OM_RIGHT);
            camMover->addCommand('8', GHInputObjectMover::OM_FORWARD);
            camMover->addCommand('2', GHInputObjectMover::OM_BACK);
            camMover->addCommand('9', GHInputObjectMover::OM_UP);
            camMover->addCommand('7', GHInputObjectMover::OM_DOWN);
            camMover->addCommand(GHKeyDef::KEY_MOUSE2, GHInputObjectMover::OM_ROTATE);
            camMover->addCommand('5', GHInputObjectMover::OM_SPRINT);
            mAppShard.mControllerMgr.add(camMover);
            mOwnershipList.push_back(new GHTypedDeletionHandle<GHInputObjectMover>(camMover));
        }
        
        mPlayerEnt->mStateMachine.setState(activeId);
    }

    /*
    GHEntity* ghamtestEnt = (GHEntity*)mAppShard.mXMLObjFactory.load("ghamtestent.xml");
    if (ghamtestEnt) {
        GHModel* ghamModel = ghamtestEnt->mProperties.getProperty(GHEntityHashes::MODEL);
        if (ghamModel)
        {
            ghamModel->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(-2, 0, 5));
        }
        ghamtestEnt->mStateMachine.setState(activeId);
        //delete ghamtestEnt;
    }
     */
}

void GHTestApp::profileGHAM(const char* srcModelName, const char* destModelName)
{
    GHPROFILESTART
    GHPROFILEBEGIN("ghamtest")
    char loadName[512];
    sprintf(loadName, "%s.xml", destModelName);
    for (int i = 0; i < 10; ++i)
    {
        GHPROFILEBEGIN("gham")
        GHModel* ghamModel = (GHModel*)mAppShard.mXMLObjFactory.load(loadName);
        ghamModel->release();
        GHPROFILEEND("gham")
        GHPROFILEBEGIN("md3")
            GHModel* md3Model = (GHModel*)mAppShard.mXMLObjFactory.load(srcModelName);
            md3Model->release();
        GHPROFILEEND("md3")
    }
    GHPROFILEEND("ghamtest");
    GHPROFILESTOP
    GHPROFILEOUTPUT
}

void GHTestApp::createGHAM(const char* srcModelName, const char* destModelName)
{
    GHModel* srcModel = (GHModel*)mAppShard.mXMLObjFactory.load(srcModelName);
    if (!srcModel) {
        GHDebugMessage::outputString("Failed to load src model for gham conversion");
        return;
    }
    
    // create a GHAM
    GHAMSerializer ghamMaker(*mSystemServices.mFileOpener, mSystemServices.mXMLSerializer);
    ghamMaker.save(*srcModel, destModelName);
    
    // load the GHAM we just created.
    char loadName[512];
    sprintf(loadName, "%s.xml", destModelName);
    GHModel* ghamModel = (GHModel*)mAppShard.mXMLObjFactory.load(loadName);
    if (ghamModel) {
        ghamModel->getSkeleton()->getLocalTransform().setTranslate(GHPoint3(1, 1, 2));
        ghamModel->addRenderableToScene(mSceneRenderer.getRenderGroup());
    }
    else {
        GHDebugMessage::outputString("failed to load gham");
        srcModel->release();
        return;
    }
    mOwnershipList.push_back(new GHRefCountedDeletionHandle(ghamModel));
    
    // compare the two.
    GHGeometryRenderable* srcRenderable = srcModel->getRenderable();
    GHGeometryRenderable* destRenderable = ghamModel->getRenderable();
    if (!srcRenderable || !destRenderable) {
        GHDebugMessage::outputString("No gham renderable to compare");
        srcModel->release();
        return;
    }
    GHGeometryComparison::compareGeometries(*srcRenderable, *destRenderable);
    
    srcModel->release();
    
    // test hack for kya legs.
    ghamModel->setAnim(mSystemServices.mHashTable.generateHash("run"));
}
