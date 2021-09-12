// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHGUIPosDesc.h"
#include <vector>

class GHIdentifier;
class GHPropertyContainer;
class GHGUIWidget;

// A controller to animate a gui widget between two positions based on a property.
class GHGUIAnim : public GHController
{
public:
    struct Frame {
        GHGUIPosDesc mPos;
        float mPropVal;
    };
    
public:
    GHGUIAnim(const GHIdentifier& prop, const GHPropertyContainer& props,
              const std::vector<GHGUIAnim::Frame>& frames,
              GHGUIWidget& widget);
    
    virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
    void findBorderFrames(float propVal, GHGUIPosDesc& lowFrame, GHGUIPosDesc& highFrame, float& pctLow);

private:
    GHGUIWidget& mWidget;
    GHIdentifier mProp;
    const GHPropertyContainer& mProps;
    // ordered list of pos frames.
    // we expect that the smallest values are in the front of the vector.
    std::vector<GHGUIAnim::Frame> mFrames;

	// last prop val we applied, used to prune out updates with no changes.
	float mLastPropVal;
	bool mLastPropValValid;
};
