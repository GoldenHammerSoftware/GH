// Copyright Golden Hammer Software
#include "GHGUISlider.h"
#include "GHGUIPosDesc.h"
#include "GHGUIWidget.h"
#include "GHUtils/GHPropertyContainer.h"
#include <algorithm>
#include "GHGUIProperties.h"
#include "GHGUICanvas.h"

GHGUISlider::GHGUISlider(GHPropertyContainer& props, const GHIdentifier& propId, GHGUIWidget& knob, 
                         const GHRect<float, 2>& bounds, const GHPoint2& range,
                         GHInputState& inputState, GHInputClaim& inputClaim, const std::vector<int>& triggerKeys)
: mProps(props)
, mPropId(propId)
, mKnob(knob)
, mBounds(bounds)
, mRange(range)
, mPointerTracker(mParentRegion, inputState, inputClaim, triggerKeys, 0, true)
{
}

void GHGUISlider::update(void)
{
    if (!mKnob.getParent()) return;
    mParentRegion = *mKnob.getParent();
    
	// if we have a canvas then redirect our pointer tracker to look at those collisions.
    IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(mKnob.getPropertyContainer());
	if (canvas)
	{
		mPointerTracker.setPointerPositionOverrides(&canvas->getPointerOverrides());
	}

    mPointerTracker.update();
    if (mPointerTracker.isSelected())
    {
        // todo: support either x or y or some combination for pct. only supporting x now.
        const GHPoint2& mousePos = mPointerTracker.getActivePosition();
        float mouseX = std::max(mBounds.mMin[0], mousePos[0]);
        mouseX = std::min(mBounds.mMax[0], mouseX);
        float mousePct = (mouseX - mBounds.mMin[0]) / (mBounds.mMax[0]-mBounds.mMin[0]);
        positionSlider(mousePct);
        updateProperty(mousePct);
    }
	// handle the property changing from an outside source
	setSliderToCurrentProp();

	mPointerTracker.setPointerPositionOverrides(0);
}

void GHGUISlider::onActivate(void)
{
	setSliderToCurrentProp();
}

void GHGUISlider::setSliderToCurrentProp(void)
{
	const GHProperty& propVal = mProps.getProperty(mPropId);
	float propFloat = (float)propVal;
	float propDist = propFloat - mRange[0];
	float propPct = propDist / (mRange[1] - mRange[0]);
	positionSlider(propPct);
}

void GHGUISlider::onDeactivate(void)
{
    mPointerTracker.onDeactivate();
}

void GHGUISlider::positionSlider(float pct)
{
    GHPoint2 pos = mBounds.mMax;
    pos -= mBounds.mMin;
    pos *= pct;
    pos += mBounds.mMin;
    GHGUIPosDesc knobDesc = mKnob.getPosDesc();
    knobDesc.mAlign = pos;
    mKnob.setPosDesc(knobDesc);
    mKnob.updatePosition();
}

void GHGUISlider::updateProperty(float pct)
{
    float newVal = mRange[0] + pct*(mRange[1]-mRange[0]);
    mProps.setProperty(mPropId, newVal);
}
