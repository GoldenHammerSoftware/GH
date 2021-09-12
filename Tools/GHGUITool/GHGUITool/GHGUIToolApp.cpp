// Copyright 2010 Golden Hammer Software
#include "GHGUIToolApp.h"
#include "GHUtils/GHMessageQueue.h"
#include "GHSystemServices.h"
#include "GHGameServices.h"
#include "GHRenderServices.h"
#include "GHGUISystem.h"
#include "GHGUIMgr.h"
#include "GHSceneRenderer.h"
#include "GHPlatform/GHDeletionHandle.h"
#include "GHNullPhysicsSim.h"
#include "GHUtils/GHTimeUpdaterAveraging.h"
#include "GTGUINavigator.h"
#include "GTInputHandler.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHRenderPass.h"
#include "GHMaterialCallbackFactoryViewInfo.h"
#include "GHMaterialCallbackFactoryTime.h"
#include "GHMaterialCallbackMgr.h"
#include "GHGUIWidgetTransition.h"
#include "GTConsoleToggler.h"
#include "GHConsoleCommandStringHandler.h"
#include "GTCommandStringHandler.h"
#include "GHXMLNode.h"
#include "GTInfoDisplayer.h"
#include "GTUndoStack.h"
#include "GTSaver.h"
#include "GTSelectionHighlighter.h"
#include "GTClearer.h"
#include "GTMetadataList.h"

#define ENUMHEADER "GTIdentifiers.h"
#include "GHString/GHEnumInclude.h"

constexpr static size_t kGUICategory = 0;

GHGUIToolApp::GHGUIToolApp(GHSystemServices& systemServices,
							GHRenderServices& renderServices,
							GHGameServices& gameServices,
							GHMessageQueue& appMessageQueue)
: mAppShard(systemServices)
, mAppMessageQueue(appMessageQueue)
, mTopLevelGUI(0)
, mCurrentlySelectedWidget(0)
{
	initBasic(systemServices, renderServices, gameServices);
	initRendering(systemServices, renderServices);
	initGUI(systemServices);
	initTool(systemServices, renderServices, appMessageQueue);

	mAppShard.mStateMachine.setState(GTIdentifiers::S_MAIN);
}

GHGUIToolApp::~GHGUIToolApp(void)
{
	mGUISystem->mGUIMgr->shutdown();
	mAppMessageQueue.clearMessages();
}

void GHGUIToolApp::runFrame(void)
{
	mAppShard.mControllerMgr.update();
}

void GHGUIToolApp::initBasic(GHSystemServices& systemServices, GHRenderServices& renderServices, GHGameServices& gameServices)
{
	systemServices.initAppShard(mAppShard);
	renderServices.initAppShard(mAppShard);

	GTIdentifiers::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());

	mSceneRenderer = new GHSceneRenderer(*renderServices.getDevice(), mAppShard.mProps);
	mAppShard.mControllerMgr.add(mSceneRenderer);

	mCamera.setNearClip(0.01f);
	mCamera.setFarClip(10.0f);
	mCamera.setFOV(60);

	mCamera.getTransform().lookAt(0, -0.3f, 0.2f,
		0, -.3f, -3.5f,
		0, 1, 0);

	GHPoint4 clearColor(0, 1, 0, 0);

	GHRenderPass* renderPass = new GHRenderPass(mCamera, renderServices.getScreenInfo(), 0,
		*renderServices.getDevice(), true,
		&clearColor, GHRenderPass::sMainPassId, false);
	mSceneRenderer->addRenderPass(renderPass);

	GHMaterialCallbackFactory* viewCB = new GHMaterialCallbackFactoryViewInfo(renderPass->getViewInfo(GHViewInfo::EYE_MONOCLE), "Main");
	renderServices.getMaterialCallbackMgr()->addFactory(viewCB);

	GHMaterialCallbackFactory* timeCB = new GHMaterialCallbackFactoryTime(mTimeVal, "Time");
	renderServices.getMaterialCallbackMgr()->addFactory(timeCB);

	mGUISystem = new GHGUISystem(systemServices, renderServices, mSceneRenderer->getRenderGroup(), mAppShard, mTimeVal);
	mGUISystem->mGUIMgr->getRenderable().setDrawOrder(11);
	mAppShard.mControllerMgr.add(mGUISystem->mGUIMgr);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHGUISystem>(mGUISystem));

	GHNullPhysicsSim* physicsSim = new GHNullPhysicsSim;
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHNullPhysicsSim>(physicsSim));
	gameServices.initAppShard(mAppShard, mTimeVal, mTimeVal, mSceneRenderer->getRenderGroup(), *physicsSim);

	GHController* realTimeUpdater = new GHTimeUpdaterAveraging(systemServices.getPlatformServices().getTimeCalculator(), mTimeVal);
	mAppShard.mControllerMgr.add(realTimeUpdater);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GHController>(realTimeUpdater));

	
}

static void addGUIToStates(const char* guiName, const GHIdentifier* states, int numStates, GHAppShard& appShard, GHGUIMgr& guiMgr,
	const GHIdentifier& transId, float transitionTime, float transitionWait)
{
	GHGUIWidgetResource* gui = (GHGUIWidgetResource*)appShard.mResourceFactory.getCacheResource(guiName);
	if (gui)
	{
		GHGUIWidgetTransition* guiTrans = new GHGUIWidgetTransition(gui, &guiMgr, GHGUITransitionDesc(transId, transitionTime, transitionWait), true, 0, kGUICategory);
		for (int i = 0; i < numStates; ++i)
		{
			appShard.mStateMachine.addTransition(states[i], guiTrans);
		}
	}
}

static void addGUIToState(const char* guiName, const GHIdentifier& state, GHAppShard& appShard, GHGUIMgr& guiMgr,
	const GHIdentifier& transId, float transitionTime, float transitionWait)
{
	addGUIToStates(guiName, &state, 1, appShard, guiMgr, transId, transitionTime, transitionWait);
}

void GHGUIToolApp::initGUI(GHSystemServices& systemServices)
{
	GHXMLNode panelNode;
	panelNode.setName("guiPanel", GHString::CHT_REFERENCE);
	mTopLevelGUI = (GHGUIWidgetResource*)mAppShard.mXMLObjFactory.load(panelNode);
	mTopLevelGUI->get()->setId(systemServices.getPlatformServices().getIdFactory().generateHash("top"));

	GHGUITransitionDesc tDesc;
	mGUISystem->mGUIMgr->pushWidget(mTopLevelGUI, tDesc, 0, kGUICategory);


	GHGUIWidgetResource* testGUI = (GHGUIWidgetResource*)mAppShard.mXMLObjFactory.load("gui.xml");
	if (testGUI) {
		mGUISystem->mGUIMgr->pushWidget(testGUI, GHGUITransitionDesc(), 0, kGUICategory);
	}

	addGUIToState("console.xml", GTIdentifiers::S_CONSOLE, mAppShard, *mGUISystem->mGUIMgr, 0, 0, 0);
}

void GHGUIToolApp::initRendering(GHSystemServices& systemServices, GHRenderServices& renderServices)
{

}

void GHGUIToolApp::initTool(GHSystemServices& systemServices, GHRenderServices& renderServices, GHMessageHandler& appMessageQueue)
{
	GTMetadataList* metadataList = new GTMetadataList;
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GTMetadataList>(metadataList));

	GTUtil* util = new GTUtil(mAppShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory(), *mTopLevelGUI, *metadataList);
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GTUtil>(util));

	GTGUINavigator* guiNavigator = new GTGUINavigator(mAppShard.mEventMgr, *mTopLevelGUI, mCurrentlySelectedWidget, *util, *metadataList, mAppShard.mProps, mGUISystem->mRectMaker, renderServices.getScreenInfo());
	mAppShard.addOwnedItem(new GHTypedDeletionHandle<GTGUINavigator>(guiNavigator));

	GTInputHandler* inputHandler = new GTInputHandler(systemServices.getInputState(), mAppShard.mEventMgr);
	mAppShard.mStateMachine.addTransition(GTIdentifiers::S_MAIN, new GHControllerTransition(mAppShard.mControllerMgr, inputHandler));

	GTConsoleToggler* consoleToggler = new GTConsoleToggler(systemServices.getInputState(), mAppShard.mStateMachine);
	mAppShard.mControllerMgr.add(consoleToggler);

	GHConsoleCommandStringHandler* commandHandler = new GHConsoleCommandStringHandler(systemServices.getInputState(),
		mAppShard.mProps,
		GTIdentifiers::GP_CONSOLEINPUTSTR,
		GTIdentifiers::GP_CONSOLERESULTSTR,
		mTimeVal,
		new GTCommandStringHandler(*mTopLevelGUI, mCurrentlySelectedWidget, systemServices.getPlatformServices().getEnumStore(), mAppShard.mProps, mGUISystem->mRectMaker, renderServices.getScreenInfo(), mAppShard.mEventMgr, systemServices.getPlatformServices().getIdFactory(), systemServices.getPlatformServices().getFilePicker(), *util, *metadataList));
	mAppShard.mStateMachine.addTransition(GTIdentifiers::S_CONSOLE, new GHControllerTransition(mAppShard.mControllerMgr, commandHandler));

	GTInfoDisplayer* infoDisplayer = new GTInfoDisplayer(mCurrentlySelectedWidget, mAppShard.mProps, *util);
	mAppShard.mControllerMgr.add(infoDisplayer);

	GTUndoStack* undoStack = new GTUndoStack(mAppShard.mEventMgr, *mTopLevelGUI, mCurrentlySelectedWidget, mGUISystem->mRectMaker, mAppShard.mProps, renderServices.getScreenInfo(), *metadataList, *util);
	mAppShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GTUndoStack>(undoStack));

	GTSaver* saver = new GTSaver(mAppShard.mEventMgr, systemServices.getXMLSerializer(), *util, *mTopLevelGUI, mAppShard.mProps, systemServices.getPlatformServices().getFilePicker(), mAppShard.mXMLObjFactory, *metadataList);
	mAppShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GTSaver>(saver));

	GTSelectionHighlighter* selectionHighlighter = new GTSelectionHighlighter(*mGUISystem->mGUIMgr, mCurrentlySelectedWidget, *util);
	mAppShard.mControllerMgr.add(selectionHighlighter);

	GTClearer* clearer = new GTClearer(*mTopLevelGUI, mCurrentlySelectedWidget);
	mAppShard.mOwnedItems->push_back(new GHTypedDeletionHandle<GTClearer>(clearer));
}
