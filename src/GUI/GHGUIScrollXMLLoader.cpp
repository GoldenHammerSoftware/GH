// Copyright Golden Hammer Software
#include "GHGUIScrollXMLLoader.h"
#include "GHGUIPanelXMLLoader.h"
#include "GHGUIPosDescXMLLoader.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHGUIWidget.h"
#include "GHXMLNode.h"
#include "GHGUIScroll.h"
#include "GHWidgetStates.h"
#include "GHGUIControllerTransition.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHGUIProperties.h"
#include "GHGUICanvas.h"

GHGUIScrollXMLLoader::GHGUIScrollXMLLoader(const GHGUIPanelXMLLoader& panelLoader,
                                           const GHGUIPosDescXMLLoader& posDescLoader,
                                           GHControllerMgr& controllerMgr,
                                           GHInputState& inputState,
                                           GHInputClaim& inputClaim,
                                           const GHScreenInfo& screenInfo,
                                           const GHTimeVal& timeVal,
										   const GHGUIRectMaker& rectMaker)
: mPanelLoader(panelLoader)
, mPosDescLoader(posDescLoader)
, mControllerMgr(controllerMgr)
, mInputState(inputState)
, mInputClaim(inputClaim)
, mScreenInfo(screenInfo)
, mTimeVal(timeVal)
, mRectMaker(rectMaker)
{
}

void* GHGUIScrollXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIPosDesc childPosDesc;
	loadChildPosDesc(childPosDesc, node, extraData);

	const GHScreenInfo* screenInfo = &mScreenInfo;
	
	const GHGUICanvas* canvas = (GHGUICanvas*)extraData.getProperty(GHGUIProperties::GP_GUICANVAS);
	if (canvas && !canvas->is2d() && canvas->getScreenInfo())
	{
		screenInfo = canvas->getScreenInfo();
	}

	GHGUIScroll* scroll = new GHGUIScroll(childPosDesc,
										  mInputState,
										  mInputClaim,
										  *screenInfo,
										  mTimeVal,
										  mRectMaker,
										  mControllerMgr);

	GHPropertyStacker scrollStack(extraData, GHGUIProperties::GP_GUISCROLL, GHProperty(scroll));

	GHGUIWidgetResource* ret = reinterpret_cast<GHGUIWidgetResource*>(mPanelLoader.create(node, extraData));
	if (!ret) {
		GHDebugMessage::outputString("Failed to load GHGUIScrollXMLLoader panel");
		delete scroll;
		return 0;
	}

	ret->get()->getPropertyContainer().setProperty(GHGUIProperties::GP_GUISCROLL, GHProperty(scroll));

	GHGUIControllerTransition* scrollTrans = new GHGUIControllerTransition(mControllerMgr, scroll, *ret->get());
	scroll->setParentWidget(ret->get());
	ret->get()->getWidgetStates().addTransition(GHWidgetState::WS_ACTIVE, scrollTrans);
	ret->get()->getWidgetStates().addTransition(GHWidgetState::WS_TRANSITIONIN, scrollTrans);

	populateScrollParams(ret, node, extraData);

	return ret;
}

void GHGUIScrollXMLLoader::populateScrollParams(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidgetResource* ret = (GHGUIWidgetResource*)obj;
	GHGUIScroll* scroll = ret->get()->getPropertyContainer().getProperty(GHGUIProperties::GP_GUISCROLL);

	float triggerThreshold = 0;
	if (node.readAttrFloat("triggerThreshold", triggerThreshold))
	{
		scroll->setTriggerThreshold(triggerThreshold);
	}

	float pointerClaimThreshold = 0;
	if (node.readAttrFloat("pointerClaimThreshold", pointerClaimThreshold))
	{
		scroll->setPointerClaimThreshold(pointerClaimThreshold);
	}

	float momentumDecayFactor = 10.f;
	if (node.readAttrFloat("momentumDecayFactor", momentumDecayFactor))
	{
		scroll->setMomentumDecayFactor(momentumDecayFactor);
	}

	float mouseWheelMultiplier = 0.1f;
	if (node.readAttrFloat("mouseWheelMultiplier", mouseWheelMultiplier))
	{
		scroll->setMouseWheelMultiplier(mouseWheelMultiplier);
	}
}

void GHGUIScrollXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	populateScrollParams(obj, node, extraData);
	mPanelLoader.populate(obj, node, extraData);
}

void GHGUIScrollXMLLoader::loadChildPosDesc(GHGUIPosDesc& posDesc, const GHXMLNode& parentNode, GHPropertyContainer& extraData) const
{
    const GHXMLNode* childPosDescNode = parentNode.getChild("childPos", false);
    if (childPosDescNode) {
        mPosDescLoader.populate(&posDesc, *childPosDescNode, extraData);
    }
}
