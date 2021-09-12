// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

// root class for transitions that show an ad
class GHAdTransition : public GHTransition
{
public:
    virtual bool hasAd(void) const = 0;
};
