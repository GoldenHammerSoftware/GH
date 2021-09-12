// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"
#include "GHMath/GHPoint.h"

class GHGUIWidget;

// A transition that modifies the size of a gui widget on activate,
//  and put it back on deactivate.
class GHGUISizeTransition : public GHTransition
{
public:
	GHGUISizeTransition(GHGUIWidget& parent, float sizeMult);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHGUIWidget& mParent;
	float mSizeMult;
	GHPoint2 mOffsetChange;
};
