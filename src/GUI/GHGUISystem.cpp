// Copyright Golden Hammer Software
#include "GHGUISystem.h"
#include "GHGUIMgr.h"
#include "GHSystemServices.h"
#include "GHRenderServices.h"
#include "GHMaterialCallbackMgr.h"
#include "GHRenderDevice.h"
#include "GHAppShard.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHDeletionHandle.h"

#include "GHGUIPosDescXMLLoader.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHGUIQuadRendererXMLLoader.h"
#include "GHGUITextXMLLoader.h"
#include "GHGUIWidgetLoader.h"
#include "GHGUIPropTextXMLLoader.h"
#include "GHGUIButtonXMLLoader.h"
#include "GHGUIJoystickXMLLoader.h"
#include "GHGUIWidgetTransitionXMLLoader.h"
#include "GHGUIPushTransitionXMLLoader.h"
#include "GHGUIPopTransitionXMLLoader.h"
#include "GHGUISliderXMLLoader.h"
#include "GHGUISwitchXMLLoader.h"
#include "GHGUITransitionControllerFactory.h"
#include "GHGUIAnimXMLLoader.h"
#include "GHInputState.h"
#include "GHWidescreenPropertySetter.h"
#include "GHGUISizeTransitionXMLLoader.h"
#include "GHGUISizeOscillateTransitionXMLLoader.h"
#include "GHGUITransitionDescXMLLoader.h"
#include "GHGUIScrollXMLLoader.h"
#include "GHGUI3DPositionFollowerXMLLoader.h"
#include "GHGUIWidgetMessageTransitionXMLLoader.h"
#include "GHGUINavMgr.h"
#include "GHGUINotchedSliderMonitorXMLLoader.h"
#include "GHGUITransitionNavResponseXMLLoader.h"
#include "GHGUISliderInputWhileSelectedXMLLoader.h"
#include "GHGUINotchedSliderNavResponseXMLLoader.h"
#include "GHGUIPoseableCanvasControllerXMLLoader.h"
#include "GHGUIMaterialCallbackFactoryTransform.h"
#include "GHGUICanvasPropertyXMLLoader.h"
#include "GHGUICanvasSwitchXMLLoader.h"
#include "GHGUICanvasMgr.h"
#include "GHGUIWidgetRendererModelXMLLoader.h"

class GHStateMachineXMLLoader;
class GHPropertyStringGeneratorXMLLoader;
#include "GHSystemServices.h"

#define ENUMHEADER "GHFillType.h"
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHButtonState.h" 
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHWidgetStates.h" 
#include "GHString/GHEnumInclude.h"
#define ENUMHEADER "GHGUIProperties.h"
#include "GHString/GHEnumInclude.h"

GHGUISystem::GHGUISystem(GHSystemServices& systemServices,
                         GHRenderServices& renderServices,
                         GHRenderGroup& sceneGroup,
                         GHAppShard& appShard,
                         const GHTimeVal& time)
: mRectMaker(renderServices.getScreenInfo())
, mInputClaim(systemServices.getInputState().getInputClaim())
, mNavMgr(systemServices.getInputState(), systemServices.getInputState().getInputClaim())
{
    mTransitionSpawner = new GHGUITransitionControllerFactory(appShard.mControllerMgr, time, appShard.mProps);
    
	GHGUICanvasMgr* canvasMgr = new GHGUICanvasMgr(renderServices.getDevice()->getViewInfo(), systemServices.getInputState());
	mOwnedItems.push_back(new GHTypedDeletionHandle<GHGUICanvasMgr>(canvasMgr));

    mGUIMgr = new GHGUIMgr(sceneGroup, 
                           appShard.mEventMgr,
                           *mTransitionSpawner,
                           appShard.mResourceFactory,
						   mInputClaim,
                           systemServices.getPlatformServices().getIdFactory(),
						   *canvasMgr,
                           mRectMaker);

	mGUIMgr->getControllerMgr().add(&mNavMgr, -2.f);
    
    GHGUIPosDescXMLLoader* posDescLoader = new GHGUIPosDescXMLLoader(systemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(posDescLoader, 1, "guiPos");

	GHGUISizeTransitionXMLLoader* sizeTransLoader = new GHGUISizeTransitionXMLLoader();
	appShard.mXMLObjFactory.addLoader(sizeTransLoader, 1, "guiSizeTransition");

	GHGUISizeOscillateTransitionXMLLoader* oscLoader = new GHGUISizeOscillateTransitionXMLLoader(time, appShard.mControllerMgr);
	appShard.mXMLObjFactory.addLoader(oscLoader, 1, "guiSizeOscillate");

    GHGUIAnimXMLLoader* animLoader = new GHGUIAnimXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory(),
                                                            *posDescLoader);
    appShard.mXMLObjFactory.addLoader(animLoader, 1, "guiAnim");
    
    GHStateMachineXMLLoader* smLoader = (GHStateMachineXMLLoader*)appShard.mXMLObjFactory.getLoader("stateMachine");
    assert(smLoader);
    GHGUIWidgetLoader* widgetLoader = new GHGUIWidgetLoader(*smLoader, appShard.mXMLObjFactory, 
                                                            systemServices.getPlatformServices().getIdFactory());
    mOwnedItems.push_back(new GHTypedDeletionHandle<GHGUIWidgetLoader>(widgetLoader));
                          
    GHGUIPanelXMLLoader* panelLoader = new GHGUIPanelXMLLoader(appShard.mXMLObjFactory, 
                                                               mGUIMgr->getRenderable(), 
                                                               mRectMaker,
                                                               *widgetLoader);
    appShard.mXMLObjFactory.addLoader(panelLoader, 1, "guiPanel");

    GHGUIQuadRendererXMLLoader* quadLoader = new GHGUIQuadRendererXMLLoader(*renderServices.getVBFactory(), 
                                                                            appShard.mXMLObjFactory, renderServices.getScreenInfo());
    appShard.mXMLObjFactory.addLoader(quadLoader, 1, "guiQuadRenderable");
    
    GHGUITextXMLLoader* textLoader = new GHGUITextXMLLoader(appShard.mResourceFactory,
                                                            systemServices.getPlatformServices().getEnumStore(),
                                                            *renderServices.getFontRenderer(),
                                                            mGUIMgr->getRenderable(),
                                                            mRectMaker,
                                                            *widgetLoader);
    appShard.mXMLObjFactory.addLoader(textLoader, 1, "guiText");
    
    GHPropertyStringGeneratorXMLLoader* stringGenLoader = 
        (GHPropertyStringGeneratorXMLLoader*)appShard.mXMLObjFactory.getLoader("propStringGenerator");
    assert(stringGenLoader);
    GHGUIPropTextXMLLoader* propTextLoader = new GHGUIPropTextXMLLoader(*textLoader, *stringGenLoader,
                                                                        appShard.mControllerMgr);
    appShard.mXMLObjFactory.addLoader(propTextLoader, 1, "guiPropText");
    
    GHFillType::generateEnum(systemServices.getPlatformServices().getEnumStore());
    GHButtonState::generateEnum(systemServices.getPlatformServices().getEnumStore());
    GHWidgetState::generateEnum(systemServices.getPlatformServices().getEnumStore());
    GHGUIProperties::generateIdentifiers(systemServices.getPlatformServices().getIdFactory());
    
    GHGUIButtonXMLLoader* buttonLoader = new GHGUIButtonXMLLoader(appShard.mXMLObjFactory, *panelLoader, *smLoader, 
                                                                  mGUIMgr->getRenderable(), mRectMaker,
                                                                  systemServices.getInputState(),
																  mInputClaim,
                                                                  mGUIMgr->getControllerMgr(),
                                                                  systemServices.getPlatformServices().getEnumStore(),
																  systemServices.getPlatformServices().getIdFactory(),
																  mNavMgr);
    appShard.mXMLObjFactory.addLoader(buttonLoader, 1, "guiButton");

	GHGUIJoystickXMLLoader* joyLoader = new GHGUIJoystickXMLLoader(mGUIMgr->getRenderable(),
		mRectMaker, *panelLoader, appShard.mProps, systemServices.getPlatformServices().getIdFactory(),
		systemServices.getInputState(), mInputClaim, appShard.mControllerMgr,
		appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(joyLoader, 1, "guiJoystick");

	GHGUITransitionDescXMLLoader* tdescLoader = new GHGUITransitionDescXMLLoader(systemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(tdescLoader, 1, "guiTDesc");

    GHGUIWidgetTransitionXMLLoader* widgetTransition = new GHGUIWidgetTransitionXMLLoader(*mGUIMgr, appShard.mResourceFactory, appShard.mXMLObjFactory, *tdescLoader);
    appShard.mXMLObjFactory.addLoader(widgetTransition, 1, "guiWidgetTransition");
 
    GHGUIPushTransitionXMLLoader* pushLoader = new GHGUIPushTransitionXMLLoader(*mGUIMgr, appShard.mResourceFactory, appShard.mXMLObjFactory, *tdescLoader);
    appShard.mXMLObjFactory.addLoader(pushLoader, 1, "guiPush");
    
    GHGUIPopTransitionXMLLoader* popLoader = new GHGUIPopTransitionXMLLoader(*mGUIMgr, systemServices.getPlatformServices().getIdFactory(), *tdescLoader);
    appShard.mXMLObjFactory.addLoader(popLoader, 1, "guiPop");
    
    GHGUISliderXMLLoader* sliderLoader = new GHGUISliderXMLLoader(appShard.mProps, 
                                                                  systemServices.getPlatformServices().getIdFactory(),
                                                                  mGUIMgr->getControllerMgr(),
                                                                  systemServices.getInputState(),
																  mInputClaim);
    appShard.mXMLObjFactory.addLoader(sliderLoader, 1, "guiSlider");
    
    GHGUISwitchXMLLoader* switchLoader = new GHGUISwitchXMLLoader(appShard.mProps,
                                                                  systemServices.getPlatformServices().getIdFactory(),
                                                                  appShard.mXMLObjFactory,
                                                                  appShard.mControllerMgr);
    appShard.mXMLObjFactory.addLoader(switchLoader, 1, "guiSwitch");

    GHGUIScrollXMLLoader* scrollLoader = new GHGUIScrollXMLLoader(*panelLoader,
                                                                  *posDescLoader,
																  mGUIMgr->getControllerMgr(),
                                                                  systemServices.getInputState(),
                                                                  mInputClaim,
                                                                  renderServices.getScreenInfo(),
                                                                  time,
																  mRectMaker);
    appShard.mXMLObjFactory.addLoader(scrollLoader, 1, "guiScroll");
    
	GHGUI3DPositionFollowerXMLLoader* pos3DFollowLoader = new GHGUI3DPositionFollowerXMLLoader(appShard.mProps, renderServices.getDevice()->getViewInfo(), systemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(pos3DFollowLoader, 1, "gui3DPosFollower");

	mOwnedItems.push_back(new GHTypedDeletionHandle<GHWidescreenPropertySetter>(new GHWidescreenPropertySetter(systemServices.getEventMgr(), renderServices.getScreenInfo(), appShard.mProps)));

	GHGUIWidgetMessageTransitionXMLLoader* wmtLoader = new GHGUIWidgetMessageTransitionXMLLoader(systemServices.getPlatformServices().getIdFactory(), appShard.mEventMgr);
	appShard.mXMLObjFactory.addLoader(wmtLoader, 1, "guiWidgetMessage");

	GHGUINotchedSliderMonitorXMLLoader* notchedSliderMonitorLoader = new GHGUINotchedSliderMonitorXMLLoader(appShard.mProps, appShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(notchedSliderMonitorLoader, 1, "guiNotchedSliderMonitor");

	GHGUITransitionNavResponseXMLLoader* transitionNavResponseLoader = new GHGUITransitionNavResponseXMLLoader(appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(transitionNavResponseLoader, 1, "guiTransitionNavResponse");

	GHGUISliderInputWhileSelectedXMLLoader* sliderNavResponseLoader = new GHGUISliderInputWhileSelectedXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory(), systemServices.getInputState(), time);
	appShard.mXMLObjFactory.addLoader(sliderNavResponseLoader, 1, "guiSliderInput");

	GHGUINotchedSliderNavResponseXMLLoader* notchedSliderNavResponseLoader = new GHGUINotchedSliderNavResponseXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(notchedSliderNavResponseLoader, 1, "guiNotchedSliderResponse");

	auto poseableCanvasLoader = new GHGUIPoseableCanvasControllerXMLLoader(systemServices.getInputState());
	appShard.mXMLObjFactory.addLoader(poseableCanvasLoader, 1, "poseableCanvasController");

	auto widgetRendererModelLoader = new GHGUIWidgetRendererModelXMLLoader(appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(widgetRendererModelLoader, 1, "guiWidgetRendererModel");

	GHGUIWidgetResource* cursorWidget = (GHGUIWidgetResource*)appShard.mXMLObjFactory.load("guinavcursor.xml");
	if (cursorWidget)
	{
		mNavMgr.setCursorWidget(cursorWidget);
	}

	GHGUIMaterialCallbackFactoryTransform* transCB = new GHGUIMaterialCallbackFactoryTransform();
	renderServices.getMaterialCallbackMgr()->addFactory(transCB);

	GHGUICanvasPropertyXMLLoader* canvasLoader = new GHGUICanvasPropertyXMLLoader(renderServices.getScreenInfo());
	appShard.mXMLObjFactory.addLoader(canvasLoader, 1, "guiCanvas");

    GHGUICanvasSwitchXMLLoader* canvasSwitchLoader = new GHGUICanvasSwitchXMLLoader(appShard.mProps, appShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(canvasSwitchLoader, 1, "guiCanvasSwitch");
}

GHGUISystem::~GHGUISystem(void)
{
	mGUIMgr->getControllerMgr().remove(&mNavMgr);

    delete mTransitionSpawner;
    delete mGUIMgr;
    
    for (size_t i = 0; i < mOwnedItems.size(); ++i) 
    {
        delete mOwnedItems[i];
    }
}
