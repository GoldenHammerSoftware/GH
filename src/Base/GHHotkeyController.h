// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>

class GHInputState;
class GHTransition;

// A controller to look for keypresses that trigger something
// we take ownership of the transition
class GHHotkeyController : public GHController
{
public:
    GHHotkeyController(const GHInputState& inputState, GHTransition* transition, bool toggleTransition=false);
    
    void addTriggerKey(unsigned int key) { mTriggerKeys.push_back(key); }
    
    virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
    const GHInputState& mInputState;
    std::vector<unsigned int> mTriggerKeys;
    GHTransition* mTransition;
	// if toggle transition, then alternate between transition->activate() and transition->deactivate()
	bool mToggleTransition{ false };
	bool mIsActive{ false };
};
