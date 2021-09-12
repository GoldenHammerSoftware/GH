// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHInputState;

//Intended to be called once per frame, existing at most once per input state,
// this clears the input state every frame.
// This class is only necessary if using a secondary input state not contolled
//  by the OS layer.
class GHInputStateClearer : public GHController
{
public:
    GHInputStateClearer(GHInputState& inputState);
    
    virtual void update(void);
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    
private:
    GHInputState& mInputState;
};
