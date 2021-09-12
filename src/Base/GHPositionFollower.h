// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"

class GHTransform;

// controller that copies the translate from one transform to another.
class GHPositionFollower : public GHController
{
public:
    GHPositionFollower(GHTransform& dest, const GHTransform& src);
    
	virtual void update(void);
	virtual void onActivate(void) {}
	virtual void onDeactivate(void) {}
    
private:
    GHTransform& mDest;
    const GHTransform& mSrc;
};
