// Copyright Golden Hammer Software
#include "GHGUIButton.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHInputState.h"
#include "GHKeyDef.h"
#include "GHGUIController.h"
#include "GHWidgetStates.h"
#include "GHGUIProperties.h"
#include "GHGUICanvas.h"
#include "GHActionClaim.h"

GHGUIButton::GHGUIButton(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker,
                         GHInputState& inputState, 
						 GHInputClaim& inputClaim,
						 GHActionClaim* actionClaim,
                         GHControllerMgr& controllerMgr,
                         const std::vector<int>& triggerKeys)
: GHGUIPanel(renderer, rectMaker)
, mNavigator(*this, actionClaim)
, mInputHandler(inputState, inputClaim, mScreenPos, mNavigator, triggerKeys)
, mControllerMgr(controllerMgr)
, mInputActive(false)
{
}

void GHGUIButton::onEnterTransitionIn(void)
{
    GHGUIPanel::onEnterTransitionIn();
    mButtonStates.setState(GHButtonState::BE_DISABLED);
}

void GHGUIButton::onFinishTransitionIn(void)
{
    mControllerMgr.add(&mInputHandler, GHGUIController::calcPriority(mDepth));
    mInputActive = true;
    GHGUIPanel::onFinishTransitionIn();
}

void GHGUIButton::onEnterTransitionOut(void)
{
    mControllerMgr.remove(&mInputHandler);
    mInputActive = false;
    GHGUIPanel::onEnterTransitionOut();
    mButtonStates.setState(GHButtonState::BE_DISABLED);
}

void GHGUIButton::onDepthChange(void)
{
    if (mInputActive) {
        mControllerMgr.updatePriority(&mInputHandler, GHGUIController::calcPriority(mDepth));
    }
}

void GHGUIButton::setButtonStateFromNav(GHButtonState::Enum state)
{
	if (mWidgetStates.getState() == GHWidgetState::WS_NONE)
	{
		// don't allow changing state while inactive.
		return;
	}
	mButtonStates.setState(state);
}

GHGUIButton::ButtonInputHandler::ButtonInputHandler(GHInputState& inputState,
													GHInputClaim& inputClaim,
                                                    const GHRect<float,2>& screenPos,
                                                    ButtonNavigator& navigator,
                                                    const std::vector<int>& triggerKeys)
: mNavigator(navigator)
, mPointerTracker(screenPos, inputState, inputClaim, triggerKeys, 0, false)
, mOwnerResource(0)
{
}

void GHGUIButton::ButtonInputHandler::update(void)
{
	IGHGUICanvas* canvas = 0;
    if (mOwnerResource) 
	{
		// In case this button triggers a remove and delete of itself,
		// make sure the delete doesn't happen until the end of this function.
		// prevents bad pointer access.
		mOwnerResource->acquire();

		// if we have a canvas then redirect our pointer tracker to look at those collisions.
		canvas = GHGUICanvas::get3dCanvas(mOwnerResource->get()->getPropertyContainer());
		if (canvas)
		{
			mPointerTracker.setPointerPositionOverrides(&canvas->getPointerOverrides());
		}
	}
    
    mPointerTracker.update();
    if (mPointerTracker.isActivated())
    {
		mNavigator.trigger();
    }
    else if (mPointerTracker.isSelected())
    {
		mNavigator.setPointerSelected(true);
    }
    else
    {
		mNavigator.setPointerSelected(false);
    }
    
    if (mOwnerResource) mOwnerResource->release();
	if (canvas) mPointerTracker.setPointerPositionOverrides(0);
}

void GHGUIButton::ButtonInputHandler::onActivate(void)
{
	mNavigator.reset();
}

void GHGUIButton::ButtonInputHandler::onDeactivate(void)
{
    mPointerTracker.onDeactivate();
	mNavigator.reset();
}

GHGUIButton::ButtonNavigator::ButtonNavigator(GHGUIButton& parent, GHActionClaim* actionClaim)
: mParent(parent)
, mActionClaim(actionClaim)
, mPriority(0)
, mPointerSelected(false)
, mNavSelected(false)
, mOverallSelected(false)
, mNavigable(true)
, mUpResponse(0)
, mRightResponse(0)
, mLeftResponse(0)
, mDownResponse(0)
, mSelectResponse(0)
{

}

void GHGUIButton::ButtonNavigator::reset(void)
{
	mParent.setButtonStateFromNav(GHButtonState::BE_IDLE);
}

void GHGUIButton::ButtonNavigator::setPointerSelected(bool selected)
{
	mPointerSelected = selected;
	handleSelectionChanged();
}

void GHGUIButton::ButtonNavigator::setNavSelected(bool selected)
{
	mNavSelected = selected;
	handleSelectionChanged();
}

void GHGUIButton::ButtonNavigator::handleSelectionChanged(void)
{
	bool isSelected = mNavSelected || mPointerSelected;
	if ((isSelected && !mOverallSelected)
		|| (!isSelected && mOverallSelected))
	{
		mOverallSelected = isSelected;
		if (mOverallSelected)
		{
			mParent.setButtonStateFromNav(GHButtonState::BE_SELECTED);
		}
		else
		{
			mParent.setButtonStateFromNav(GHButtonState::BE_IDLE);
		}
	}
}

void GHGUIButton::ButtonNavigator::trigger(void)
{
	if (mActionClaim && !mActionClaim->claimAction())
	{
		return;
	}

	mParent.setButtonStateFromNav(GHButtonState::BE_TRIGGERED);
	mOverallSelected = false;
	handleSelectionChanged();
}
