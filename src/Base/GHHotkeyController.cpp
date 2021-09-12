// Copyright Golden Hammer Software
#include "GHHotkeyController.h"
#include "GHInputState.h"
#include "GHUtils/GHTransition.h"
#include <stddef.h>

GHHotkeyController::GHHotkeyController(const GHInputState& inputState, 
                                        GHTransition* transition,
										bool toggleTransition)
: mInputState(inputState)
, mTransition(transition)
, mToggleTransition(toggleTransition)
{
}

void GHHotkeyController::update(void)
{
    for (size_t i = 0; i < mTriggerKeys.size(); ++i)
    {
        if (mInputState.checkKeyChange(0, mTriggerKeys[i], false))
        {
			if (mToggleTransition) {
				if (mIsActive) {
					mTransition->deactivate();
				}
				else {
					mTransition->activate();
				}
				mIsActive = !mIsActive;
			}
			else {
				mTransition->activate();
				mIsActive = true;
			}
            return;
        }
    }
}

