// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHString/GHIdentifier.h"
#include "GHMath/GHRect.h"
#include "GHPointerRegionTracker.h"

class GHPropertyContainer;
class GHGUIWidget;
class GHInputClaim;

// A controller to manage the position of a slider knob
// We move the knob between bounds.min and bounds.max by setting the anchor point
// We also trap input when the mouse is inside background
class GHGUISlider : public GHController
{
public:
    GHGUISlider(GHPropertyContainer& props, const GHIdentifier& propId, GHGUIWidget& knob, 
                const GHRect<float, 2>& bounds, const GHPoint2& range,
                GHInputState& inputState, GHInputClaim& inputClaim, const std::vector<int>& triggerKeys);
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);
    
private:
    void positionSlider(float pct);
    void updateProperty(float pct);
	void setSliderToCurrentProp(void);

private:
    GHRect<float,2> mParentRegion;
    GHPointerRegionTracker mPointerTracker;
    
    GHPropertyContainer& mProps;
    GHIdentifier mPropId;
    // the knob that we move around
    // the knob's parent is used for input range
    GHGUIWidget& mKnob;
    // the min and max position of the knob
    GHRect<float, 2> mBounds;
    // the min and max values for the property.
    GHPoint2 mRange;
};
