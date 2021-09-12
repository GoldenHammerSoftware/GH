// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHString/GHIdentifier.h"

class GHModel;
class GHRenderGroup;

// A class to make an entity visible when it is active
class GHModelRenderTransition : public GHTransition
{
public:
    GHModelRenderTransition(GHModel& model, GHRenderGroup& renderGroup, const GHIdentifier& renderType);
    
    virtual void activate(void);
	virtual void deactivate(void);
    
private:
    GHModel& mModel;
    GHRenderGroup& mRenderGroup;
    GHIdentifier mRenderType;
};
