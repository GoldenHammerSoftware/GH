// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHController.h"

class GHRagdollGenerator;
class GHInputState;
class GHPropertyContainer;
class GHXMLSerializer;

class GHRagdollToolSaveInputHandler : public GHController
{
public:
    GHRagdollToolSaveInputHandler(const GHXMLSerializer& xmlSerializer,
                                  const GHInputState& inputState,
                                  GHRagdollGenerator& ragdollGenerator,
                                  GHPropertyContainer& props);
    
    virtual void onActivate(void);
    virtual void onDeactivate(void);
    virtual void update(void);

private:
    const GHXMLSerializer& mSerializer;
    const GHInputState& mInputState;
    GHRagdollGenerator& mRagdollGenerator;
    GHPropertyContainer& mProps;
};
