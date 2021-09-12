// Copyright Golden Hammer Software
#pragma once

#include "GHGUIPanel.h"
#include "GHUtils/GHController.h"
#include "GHButtonState.h"
#include "GHUtils/GHStateMachine.h"
#include <vector>
#include "Base/GHPointerRegionTracker.h"

class GHInputState;
class GHControllerMgr;
class GHInputClaim;
class GHGUINavResponse;
class GHActionClaim;

// A button is something that can be put in the gui that reacts to
// being selected and triggered.
class GHGUIButton : public GHGUIPanel
{
public:
	// inner class to manage button state machine
	class ButtonNavigator
	{
	public:
		ButtonNavigator(GHGUIButton& parent, GHActionClaim* actionClaim);
		
		void reset(void);
		void setPointerSelected(bool selected);
		void setNavSelected(bool selected);
		void trigger(void);
		bool isNavigable(void) const { return mNavigable; }
		void setNavigable(bool navigable) { mNavigable = navigable; }
		float getPriority(void) const { return mPriority; }
		void setPriority(float pri) { mPriority = pri; }

		GHGUINavResponse* mUpResponse;
		GHGUINavResponse* mDownResponse;
		GHGUINavResponse* mLeftResponse;
		GHGUINavResponse* mRightResponse;
		GHGUINavResponse* mSelectResponse;

	private:
		void handleSelectionChanged(void);

	private:
		GHGUIButton& mParent;
        GHActionClaim* mActionClaim{ nullptr };
		float mPriority;
		bool mPointerSelected;
		bool mNavSelected;
		bool mOverallSelected;
		bool mNavigable;
	};
	ButtonNavigator& getNavigator(void) { return mNavigator; }

private:
    // inner class to handle looking at input to see if we should trigger the button.
    class ButtonInputHandler : public GHController
    {
    public:
        ButtonInputHandler(GHInputState& inputState,
						   GHInputClaim& pointerClaim,
                           const GHRect<float,2>& screenPos,
                           ButtonNavigator& navigator,
                           const std::vector<int>& triggerKeys);
        
        virtual void update(void);
        virtual void onActivate(void);
        virtual void onDeactivate(void);
        
        void addTriggerKey(int key) { mPointerTracker.addTriggerKey(key); }
        void setOwnerResource(GHGUIWidgetResource* res) { mOwnerResource = res; }
        
    private:
        GHPointerRegionTracker mPointerTracker;
		ButtonNavigator& mNavigator;
        GHGUIWidgetResource* mOwnerResource;
    };
    
public:
    GHGUIButton(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker,
                GHInputState& inputState,
				GHInputClaim& pointerClaim,
                GHActionClaim* actionClaim,
                GHControllerMgr& controllerMgr,
                const std::vector<int>& triggerKeys);
        
    // for loading use only
    GHStateMachine& getButtonStates(void) { return mButtonStates; }
    void addTriggerKey(int key) { mInputHandler.addTriggerKey(key); }

    // set a pointer to the owning resource, 
    // helps prevent us from going out of scope during button handling.
    void setOwnerResource(GHGUIWidgetResource* res) { mInputHandler.setOwnerResource(res); }
    
protected:
    virtual void onEnterTransitionIn(void);
    virtual void onFinishTransitionIn(void);
    virtual void onEnterTransitionOut(void);
    virtual void onDepthChange(void);
	// handle a request from ButtonNavigator to set the button state.
	void setButtonStateFromNav(GHButtonState::Enum state);

private:
	GHStateMachine mButtonStates;
	ButtonNavigator mNavigator;
    ButtonInputHandler mInputHandler;
    GHControllerMgr& mControllerMgr;
    bool mInputActive;
};
