// Copyright 2010 Golden Hammer Software
#include "MCApp.h"
#include "Base/GHSystemServices.h"
#include "Render/GHRenderServices.h"
#include "Game/GHGameServices.h"
#include "GHBullet/GHBulletPhysicsServices.h"
#include "sim/physics/GHPhysicsSim.h"
#include "GHUtils/GHProfiler.h"
#include "MCRenderer.h"

#include "GHPlatform/GHDeletionHandle.h"
#include "GHUtils/GHStateChangeMessageListener.h"
#include "GUI/GHGUISystem.h"
#include "GUI/GHGUIMgr.h"
#include "GHPlatform/GHTimeCalculator.h"
#include "Render/GHSceneRenderer.h"
#include "GUI/GHGUIWidgetTransition.h"
#include "GHUtils/GHControllerTransition.h"
#include "Base/GHSoundPropertyVolumeChanger.h"
#include "GHUtils/GHTimeUpdaterAveraging.h"
#include "Base/GHFramerateMonitor.h"

#define ENUMHEADER "MCIdentifiers.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "MCGameStates.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "MCUpdatePriority.h"
#include "GHString/GHEnumInclude.h"

MCApp::MCApp(GHSystemServices& systemServices,
	GHRenderServices& renderServices,
	GHGameServices& gameServices)
: mAppShard(systemServices)
{
	initBasic(systemServices, renderServices, gameServices);
	initFPSCounter(systemServices);
	initMCLoaders(systemServices, renderServices);
	initGUI(systemServices);
	initDebug(systemServices, renderServices);

	GHStateChangeMessageListener* scml = new GHStateChangeMessageListener(mAppShard.mStateMachine, 0, MCIdentifiers::GP_STATE);
	mAppShard.mEventMgr.registerListener(MCIdentifiers::M_CHANGEMAINSTATE, *scml);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHStateChangeMessageListener>(scml));

	mAppShard.mStateMachine.setState(MCGameStates::GS_MENU);
}

MCApp::~MCApp(void)
{
	mGUISystem->mGUIMgr->shutdown();
}

void MCApp::runFrame(void)
{
	GHPROFILEBEGIN("main")
	mAppShard.mControllerMgr.update();
	GHPROFILEEND("main")
}

void MCApp::initBasic(GHSystemServices& systemServices,
	GHRenderServices& renderServices,
	GHGameServices& gameServices)
{
	systemServices.initAppShard(mAppShard);
	renderServices.initAppShard(mAppShard);

	MCIdentifiers::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
	MCGameStates::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
	MCUpdatePriority::generateEnum(systemServices.getPlatformServices().getEnumStore());

	mAppShard.mProps.setProperty(MCIdentifiers::GP_GAMEVERSION, "0.0");

	srand((unsigned int)(systemServices.getPlatformServices().getTimeCalculator().calcTime()*(float)INT_MAX));

	mSceneRenderer = new GHSceneRenderer(*renderServices.getDevice(), mAppShard.mProps);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHSceneRenderer>(mSceneRenderer));

	mPhysicsServices = new GHBulletPhysicsServices(systemServices, GHPoint3(0, -9.8f, 0));
	mPhysicsSim = mPhysicsServices->createPhysicsSim(mSimTimeVal);
	mPhysicsServices->initAppShard(mAppShard, *mPhysicsSim, systemServices.getPlatformServices().getEnumStore());
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHPhysicsServices>(mPhysicsServices));

	//mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHPhysicsSim>(mPhysicsSim));
	GHControllerTransition* physicsTransition = new GHControllerTransition(mAppShard.mControllerMgr, mPhysicsSim, MCUpdatePriority::UP_PHYSICS);
	mAppShard.mStateMachine.addTransition(MCGameStates::GS_GAME, physicsTransition);

	mGUISystem = new GHGUISystem(systemServices, renderServices, mSceneRenderer->getRenderGroup(), mAppShard, mRealTimeVal);
	mGUISystem->mGUIMgr->getRenderable().setDrawOrder(11);
	mAppShard.mControllerMgr.add(mGUISystem->mGUIMgr);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHGUISystem>(mGUISystem));
	gameServices.initAppShard(mAppShard, mRealTimeVal, mSimTimeVal, mSceneRenderer->getRenderGroup(), *mPhysicsSim);

	GHController* realTimeUpdater = new GHTimeUpdaterAveraging(systemServices.getPlatformServices().getTimeCalculator(), mRealTimeVal);
	mAppShard.mControllerMgr.add(realTimeUpdater, MCUpdatePriority::UP_TIME);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHController>(realTimeUpdater));

	GHTransition* simTimeUpdater = new GHControllerTransition(mAppShard.mControllerMgr,
		new GHTimeUpdaterAveraging(systemServices.getPlatformServices().getTimeCalculator(), mSimTimeVal));
	mAppShard.mStateMachine.addTransition(MCGameStates::GS_GAME, simTimeUpdater);

	GHSoundPropertyVolumeChanger* volumeChanger = new GHSoundPropertyVolumeChanger(mAppShard.mProps, MCIdentifiers::GP_VOLUME, 
		GHIdentifier(0), systemServices.getSoundVolumeMgr());
	mAppShard.mControllerMgr.add(volumeChanger);

	mMCRenderer = new MCRenderer(*mSceneRenderer, mAppShard,
		mRealTimeVal, renderServices, systemServices);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<MCRenderer>(mMCRenderer));
}

void MCApp::initFPSCounter(GHSystemServices& systemServices)
{
	GHIdentifier fpsProp = systemServices.getPlatformServices().getIdFactory().generateHash("FPS");
	GHIdentifier msProp = systemServices.getPlatformServices().getIdFactory().generateHash("MS");
	GHFramerateMonitor* fpsMon = new GHFramerateMonitor(mRealTimeVal, mAppShard.mProps,
		fpsProp, msProp);
	mAppShard.mControllerMgr.add(fpsMon);

#ifndef GHRETAIL
	GHGUIWidgetResource* fpsGUI = (GHGUIWidgetResource*)mAppShard.mResourceFactory.getCacheResource("fpsGUI.xml");
	if (fpsGUI) {
		mGUISystem->mGUIMgr->pushWidget(fpsGUI, GHGUITransitionDesc(), 2, 0);
	}
#endif
}

static void addGUIToStates(const char* guiName, const GHIdentifier& state, GHAppShard& appShard, GHGUIMgr& guiMgr,
	const GHIdentifier& transId, float transitionTime, float transitionWait)
{
	GHPROFILESCOPE(guiName, GHString::CHT_COPY);
	GHGUIWidgetResource* gui = (GHGUIWidgetResource*)appShard.mResourceFactory.getCacheResource(guiName);
	if (gui)
	{
		GHGUIWidgetTransition* guiTrans = new GHGUIWidgetTransition(gui, &guiMgr, GHGUITransitionDesc(transId, transitionTime, transitionWait), true, 0, 0);
		appShard.mStateMachine.addTransition(state, guiTrans);
	}
	else
	{
		GHDebugMessage::outputString("Failed to load gui %s", guiName);
	}
}

void MCApp::initGUI(GHSystemServices& systemServices)
{
	GHIdentifier menuProp = systemServices.getPlatformServices().getIdFactory().generateHash("GP_MENUTRANS");
	addGUIToStates("menuGUI.xml", MCGameStates::GS_MENU, mAppShard, *mGUISystem->mGUIMgr, menuProp, 0.4f, 0);

	GHIdentifier hudProp = systemServices.getPlatformServices().getIdFactory().generateHash("GP_HUDTRANS");
	addGUIToStates("hudGUI.xml", MCGameStates::GS_GAME, mAppShard, *mGUISystem->mGUIMgr, hudProp, 0.4f, 0);

	GHIdentifier pauseProp = systemServices.getPlatformServices().getIdFactory().generateHash("GP_PAUSETRANS");
	addGUIToStates("pauseGUI.xml", MCGameStates::GS_PAUSE, mAppShard, *mGUISystem->mGUIMgr, pauseProp, 0.4f, 0);
}

void MCApp::initDebug(GHSystemServices& systemServices, GHRenderServices& renderServices)
{
}

void MCApp::initMCLoaders(GHSystemServices& systemServices, GHRenderServices& renderServices)
{
}
