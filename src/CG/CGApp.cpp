// Copyright 2010 Golden Hammer Software
#include "CGApp.h"
#include "GHRenderServices.h"
#include "GHSystemServices.h"
#include "GHBulletPhysicsServices.h"
#include "GHGameServices.h"
#include "GHGUISystem.h"
#include "GHRenderPass.h"
#include "GHGUIMgr.h"
#include "GHTimeCalculator.h"
#include "GHFramerateMonitor.h"
#include "GHPi.h"
#include "GHMaterialCallbackFactory.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHMaterialCallbackMgr.h"
#include "GHLightMgr.h"
#include "GHDebugDraw.h"
#include "GHTimeUpdater.h"
#include "GHInputObjectMover.h"
#include "GHKeyDef.h"
#include "GHPhysicsSim.h"
#include "GHEntityHashes.h"
#include "GHGUIWidgetTransition.h"
#include "CGCharacterControllerXMLLoader.h"
#include "CGProperties.h"
#include "GHStateChangeMessageListener.h"
#include "GHHotkeyController.h"
#include "GHStateChangeTransition.h"
#include "GHControllerTransition.h"
#include "CGGame.h"
#include "CGCharacterAnimControllerXMLLoader.h"
#include "CGCharacterBehaviorMonitorXMLLoader.h"
#include "CGInputHandler.h"
#include "GHRenderDevice.h"
#include "GHMaterialCallbackFactoryProperty.h"
#include "CGRayPicker.h"
#include "GHRenderTargetFactory.h"
#include "GHRenderTargetMaterialCallbackFactory.h"
#include "GHRenderTarget.h"
#include "CGBaseXMLLoader.h"
#include "GHPropertyTypeXMLLoader.h"
#include "CGBase.h"
#include "GHMaterialCallbackType.h"

#define ENUMHEADER "CGGameStates.h"
#include "GHEnumInclude.h"
#define ENUMHEADER "CGProperties.h" 
#include "GHEnumInclude.h"

CGApp::CGApp(GHSystemServices& systemServices,
                           GHRenderServices& renderServices,
                           GHGameServices& gameServices,
                           GHMessageQueue& messageQueue)
: mSystemServices(systemServices)
, mRenderServices(renderServices)
, mPhysicsServices(0)
, mPhysicsSim(0)
, mAppShard(systemServices)
, mSceneRenderer(*renderServices.getDevice())
{
    generateEnumCGGameStates(mSystemServices.mEnumStore);
    CGProperties::generateIdentifiers(mSystemServices.mHashTable);
    
    mSystemServices.initAppShard(mAppShard);
    mRenderServices.initAppShard(mAppShard);
    mPhysicsServices = new GHBulletPhysicsServices(mTimeVal);
    mPhysicsSim = mPhysicsServices->createPhysicsSim();
    mPhysicsServices->initAppShard(mAppShard, *mPhysicsSim, mSystemServices.mEnumStore);
    mAppShard.mControllerMgr.add(mPhysicsSim);
    
    mGUISystem = new GHGUISystem(mSystemServices, renderServices, mSceneRenderer.getRenderGroup(), mAppShard, mTimeVal);
    mGUISystem->mGUIMgr->getRenderable().setDrawOrder(11);
    mAppShard.mControllerMgr.add(mGUISystem->mGUIMgr);
    
    gameServices.initAppShard(mAppShard, mTimeVal, mSceneRenderer.getRenderGroup(), *mPhysicsSim);
    
    srand (mSystemServices.mTimeCalc->calcTime()*INT_MAX);

    mRayPicker = new CGRayPicker(mRenderServices.getDevice()->getViewInfo(),
                                 *mPhysicsSim);
    mOwnershipList.add(new GHTypedDeletionHandle<CGRayPicker>(mRayPicker));
    
    mGame = new CGGame(mAppShard, systemServices, mGameInputStateMachine, *mRayPicker, mCamera);

    GHController* timeUpdater = new GHTimeUpdater(*mSystemServices.mTimeCalc, mTimeVal);
    mAppShard.mControllerMgr.add(timeUpdater);
    
    initRendering(renderServices);
    
    CGCharacterControllerXMLLoader* characterControllerLoader = new CGCharacterControllerXMLLoader(mAppShard.mControllerMgr);
    mAppShard.mXMLObjFactory.addLoader(characterControllerLoader, 1, "cgCharacterController");
    
    CGCharacterAnimControllerXMLLoader* characterAnimLoader = new CGCharacterAnimControllerXMLLoader(mSystemServices.mHashTable, mAppShard.mControllerMgr);
    mAppShard.mXMLObjFactory.addLoader(characterAnimLoader, 1, "cgCharacterAnimController");
    
    CGCharacterBehaviorMonitorXMLLoader* behaviorMonitorLoader = new CGCharacterBehaviorMonitorXMLLoader(mTimeVal, mAppShard.mControllerMgr);
    mAppShard.mXMLObjFactory.addLoader(behaviorMonitorLoader, 1, "cgBehaviorMonitor");
    
    CGBaseXMLLoader* baseLoader = new CGBaseXMLLoader(mAppShard.mXMLObjFactory, mSystemServices.mHashTable);
    mAppShard.mXMLObjFactory.addLoader(baseLoader, 1, "cgBase");
    
    GHXMLObjLoader* basePropLoader = new GHPropertyTypeXMLLoader<CGBase>(mAppShard.mXMLObjFactory);
    mAppShard.mXMLObjFactory.addLoader(basePropLoader, 1, "cgBaseProp");
    
    GHIdentifier fpsProp = mSystemServices.mHashTable.generateHash("FPS");
    GHIdentifier msProp = mSystemServices.mHashTable.generateHash("MS");
    GHFramerateMonitor* fpsMon = new GHFramerateMonitor(mTimeVal, mAppShard.mProps,
                                                        fpsProp, msProp);
    mAppShard.mControllerMgr.add(fpsMon);
    
    GHIdentifier scArg = mSystemServices.mHashTable.generateHash("state");
    GHStateChangeMessageListener* scml = new GHStateChangeMessageListener(mAppShard.mStateMachine, scArg);
    mAppShard.mEventMgr.registerListener(CGProperties::CHANGEMAINSTATE, *scml);
    mOwnershipList.add(new GHTypedDeletionHandle<GHStateChangeMessageListener>(scml));

    initMenuStates();
    initGameStates();
    
    initDebugCam();
    mAppShard.mStateMachine.setState(GS_MENU);
}

void CGApp::initRendering(GHRenderServices& renderServices)
{
    mCamera.setNearClip(0.2f);
    mCamera.setFarClip(100.0f);
    
    float aspectRatio = mRenderServices.getScreenInfo().getAspectRatio();
    float FOV = 60.0f;
    mCamera.setFOV(FOV);

    // Shadow
    mShadowCamera.setFOV(1.0f);
    mShadowCamera.setNearClip(200);
    mShadowCamera.setFarClip(1000);
    mShadowCamera.getTransform() = GHTransform(0.983977,
                                               0,
                                               -0.178294,
                                               0,
                                               0.175437,
                                               0.178315,
                                               0.968207,
                                               0,
                                               0.0317926,
                                               -0.983973,
                                               0.175458,
                                               0,
                                               -27.768,
                                               855.189,
                                               -152.475,
                                               1);
    
    int shadowSize = 2048;
    GHPoint4 shadowClear(0,0,0,0);
    GHRenderTarget* shadowTarget = mRenderServices.getRenderTargetFactory()->createRenderTarget(shadowSize,shadowSize);
    mOwnershipList.add(new GHTypedDeletionHandle<GHRenderTarget>(shadowTarget));
    GHIdentifier shadowId = mSystemServices.mHashTable.generateHash("shadow");
    GHRenderPass* shadowPass = new GHRenderPass(mShadowCamera, renderServices.getScreenInfo(), 
                                                shadowTarget, *mRenderServices.getDevice(), 
                                                true, &shadowClear, shadowId);
    mSceneRenderer.addRenderPass(shadowPass);
    GHMaterialCallbackFactory* shadowCallback = 
    new GHRenderTargetMaterialCallbackFactory("ShadowTexture", 
                                              GHMaterialCallbackType::CT_PERFRAME, 
                                              *shadowTarget);
    renderServices.getMaterialCallbackMgr()->addFactory(shadowCallback);
    
    GHMaterialCallbackFactory* shadowViewCB = new GHMaterialCallbackFactoryViewInfo(shadowPass->getViewInfo(), "Shadow");
    mRenderServices.getMaterialCallbackMgr()->addFactory(shadowViewCB);

    // Main
    mCamera.getTransform() = GHTransform(1,
                                         0,
                                         -0.000966668,
                                         0,
                                         0.000946142,
                                         0.204978,
                                         0.978766,
                                         0,
                                         0.000198146,
                                         -0.978767,
                                         0.204978,
                                         0,
                                         -0.0973846,
                                         10.3368,
                                         -3.34673,
                                         1);
    
    GHPoint4 clearColor(0,0,1,0);
    
    GHRenderPass* renderPass = new GHRenderPass(mCamera, renderServices.getScreenInfo(), 0,
                                                *mRenderServices.getDevice(), true, &clearColor,
                                                GHRenderPass::sMainPassId);
    mSceneRenderer.addRenderPass(renderPass);
    mAppShard.mControllerMgr.add(&mSceneRenderer);
    
    GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderPass->getViewInfo(), "Main");
    mRenderServices.getMaterialCallbackMgr()->addFactory(viewCB);
    
    GHMaterialCallbackFactory* moveCenterCallback = new GHMaterialCallbackFactoryProperty("movecircle",GHMaterialCallbackType::CT_PERFRAME, CGProperties::GP_MOVECIRCLE, GHMaterialParamHandle::HT_VEC3, &mAppShard.mProps);
    mRenderServices.getMaterialCallbackMgr()->addFactory(moveCenterCallback);
        
    mRenderServices.getLightMgr().setLightDir(0, GHPoint3(-0.3,1,-0.8));
    
    GHMaterial* debugMat = (GHMaterial*)mAppShard.mXMLObjFactory.load("debugmat.xml");
    GHDebugDraw* debugDraw = new GHDebugDraw(*mRenderServices.getVBFactory(), debugMat);
    debugDraw->setDrawOrder(10);
    GHDebugDraw::setSingleton(debugDraw);
    mSceneRenderer.getRenderGroup().addRenderable(GHDebugDraw::getSingleton());
    
    GHMaterialCallbackFactory* entColorCallback = new GHMaterialCallbackFactoryProperty("EntColor",GHMaterial::CT_PERENT, CGProperties::PP_ENTCOLOR, GHMaterialParamHandle::HT_VEC3);
    mRenderServices.getMaterialCallbackMgr()->addFactory(entColorCallback);
}

void CGApp::initDebugCam(void)
{
    GHStateChangeTransition* fromDebugTransition = new GHStateChangeTransition(mAppShard.mStateMachine, GS_GAME);
    GHHotkeyController* fromDebugChanger = new GHHotkeyController(*mSystemServices.mInputState, fromDebugTransition);
    fromDebugChanger->addTriggerKey('`');
    mAppShard.mStateMachine.addTransition(GS_DEBUG, new GHControllerTransition(mAppShard.mControllerMgr, fromDebugChanger));
    
    GHStateChangeTransition* toDebugTransition = new GHStateChangeTransition(mAppShard.mStateMachine, GS_DEBUG);
    GHHotkeyController* toDebugChanger = new GHHotkeyController(*mSystemServices.mInputState, toDebugTransition);
    toDebugChanger->addTriggerKey('`');
    mAppShard.mStateMachine.addTransition(GS_GAME, new GHControllerTransition(mAppShard.mControllerMgr, toDebugChanger));
    
    // *** DEBUG CAM!
    GHInputObjectMover* camMover = new GHInputObjectMover(mCamera.getTransform(), 
                                                          *mSystemServices.mInputState, mTimeVal);
    camMover->setMoveSpeed(5.0f);
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
    
    //Dvorak controls
    camMover->addCommand('e', GHInputObjectMover::OM_RIGHT);
    camMover->addCommand(',', GHInputObjectMover::OM_FORWARD);
    camMover->addCommand('o', GHInputObjectMover::OM_BACK);
    camMover->addCommand('j', GHInputObjectMover::OM_DOWN);
    
    camMover->addCommand(GHKeyDef::KEY_MOUSE1, GHInputObjectMover::OM_ROTATE);
    camMover->addCommand(GHKeyDef::KEY_SHIFT, GHInputObjectMover::OM_SPRINT);
    
    mAppShard.mStateMachine.addTransition(GS_DEBUG, new GHControllerTransition(mAppShard.mControllerMgr, camMover));
}

static void addGUIToStates(const char* guiName, int* states, int numStates, GHAppShard& appShard, GHGUIMgr& guiMgr,
                           const GHIdentifier& transId, float transitionTime, float transitionWait)
{
    GHGUIWidgetResource* gui = (GHGUIWidgetResource*)appShard.mResourceFactory.getCacheResource(guiName);
    if (gui) 
    {
        GHGUIWidgetTransition* guiTrans = new GHGUIWidgetTransition(gui, 0, guiMgr, transId, transitionTime, transitionWait);
        for (int i = 0; i < numStates; ++i)
        {
            appShard.mStateMachine.addTransition(states[i], guiTrans);
        }
    }
}

static void addGUIToState(const char* guiName, int state, GHAppShard& appShard, GHGUIMgr& guiMgr,
                          const GHIdentifier& transId, float transitionTime, float transitionWait)
{
    addGUIToStates(guiName, &state, 1, appShard, guiMgr, transId, transitionTime, transitionWait);
}

void CGApp::initMenuStates(void)
{
    GHGUIWidgetResource* fpsGUI = (GHGUIWidgetResource*)mAppShard.mResourceFactory.getCacheResource("fpsGUI.xml");
    if (fpsGUI) {
        mGUISystem->mGUIMgr->pushWidget(fpsGUI);
    }
    addGUIToState("hudGUI.xml", GS_GAME, mAppShard, *mGUISystem->mGUIMgr, mSystemServices.mHashTable.generateHash("GP_HUDTRANS"), 0.4, 0);
    addGUIToState("pauseGUI.xml", GS_PAUSE, mAppShard, *mGUISystem->mGUIMgr, mSystemServices.mHashTable.generateHash("GP_PAUSETRANS"), 0.4, 0);
    addGUIToState("menuGUI.xml", GS_MENU, mAppShard, *mGUISystem->mGUIMgr, mSystemServices.mHashTable.generateHash("GP_MENUTRANS"), 0.4, 0);
}

void CGApp::initGameStates(void)
{
    mAppShard.mStateMachine.addTransition(GS_GAME, new GHStateChangeTransition(mGameInputStateMachine, GS_DEFAULT));
    mAppShard.mStateMachine.addTransition(GS_PAUSE, new GHStateChangeTransition(mGameInputStateMachine, -1));
    mAppShard.mStateMachine.addTransition(GS_DEBUG, new GHStateChangeTransition(mGameInputStateMachine, -1));
}

CGApp::~CGApp(void)
{
    delete mGame;
    mGUISystem->mGUIMgr->shutdown();
    mAppShard.mStateMachine.shutdown();
    mSceneRenderer.getRenderGroup().removeRenderable(GHDebugDraw::getSingleton());
    delete mGUISystem;
    delete mPhysicsServices;
    GHDebugDraw::setSingleton(0);
}

void CGApp::runFrame(void)
{
    mAppShard.mControllerMgr.update();
}

void CGApp::osFrameUpdate(void)
{
    mAppShard.mOSFrameControllerMgr.update();
}
