// Copyright 2010 Golden Hammer Software
#pragma  once

#include "GHController.h"

class GHRagdollToolNode;
class GHInputState;
class GHPropertyContainer;

class GHRagdollToolOffsetAdjuster : public GHController
{
public:
    GHRagdollToolOffsetAdjuster(const GHInputState& inputState,
                                GHRagdollToolNode*& currentSelection,
                                GHPropertyContainer& props);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);
    
private:
    const GHInputState& mInputState;
    GHRagdollToolNode*& mCurrentSelection;
    GHPropertyContainer& mProps;
    
    const static int BUFLEN = 256;
    char mMessageBuf[BUFLEN];
};
