// Copyright Golden Hammer Software
#include "GHGUIAnim.h"
#include "GHUtils/GHPropertyContainer.h"
#include "assert.h"
#include "GHGUIWidget.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUIAnim::GHGUIAnim(const GHIdentifier& prop, const GHPropertyContainer& props,
                     const std::vector<GHGUIAnim::Frame>& frames,
                     GHGUIWidget& widget)
: mProp(prop)
, mProps(props)
, mFrames(frames)
, mWidget(widget)
, mLastPropValValid(false)
{
    assert(mFrames.size());
}

void GHGUIAnim::update(void)
{
    float propVal = mProps.getProperty(mProp);

	// filter out calls with no changes, otherwise we are uploading a ton of stuff to the gpu every frame.
    bool propChanged = (!mLastPropValValid || propVal != mLastPropVal);
	mLastPropVal = propVal;
	mLastPropValValid = true;
	if (!propChanged)
	{
		return;
	}

    GHGUIPosDesc lowFrame, highFrame;
    float pctLow;
    findBorderFrames(propVal, lowFrame, highFrame, pctLow);
    
    GHGUIPosDesc newDesc = mWidget.getPosDesc();
    
    lowFrame.mOffset *= pctLow;
    highFrame.mOffset *= (1.0f-pctLow);
    lowFrame.mOffset += highFrame.mOffset;
    newDesc.mOffset = lowFrame.mOffset;
    
    lowFrame.mAlign *= pctLow;
    highFrame.mAlign *= (1.0f-pctLow);
    lowFrame.mAlign += highFrame.mAlign;
    newDesc.mAlign = lowFrame.mAlign;

    lowFrame.mSize *= pctLow;
    highFrame.mSize *= (1.0f-pctLow);
    lowFrame.mSize += highFrame.mSize;
    newDesc.mSize = lowFrame.mSize;

    lowFrame.mAngleRadians *= pctLow;
    highFrame.mAngleRadians *= (1.0f - pctLow);
    newDesc.mAngleRadians = lowFrame.mAngleRadians + highFrame.mAngleRadians;
    
    mWidget.setPosDesc(newDesc);
    mWidget.updatePosition();
}

void GHGUIAnim::onActivate(void)
{
    mLastPropValValid = false;
    update();
}
void GHGUIAnim::onDeactivate(void)
{
    update();
}

void GHGUIAnim::findBorderFrames(float propVal, GHGUIPosDesc& lowFrame, GHGUIPosDesc& highFrame, float& pctLow)
{
    lowFrame = mFrames[0].mPos;
    float lowFrameVal = mFrames[0].mPropVal;
    std::vector<GHGUIAnim::Frame>::const_iterator frameIter;
    for (frameIter = mFrames.begin(); frameIter != mFrames.end(); ++frameIter)
    {
        if ((*frameIter).mPropVal >= propVal) {
            highFrame = (*frameIter).mPos;
            float distFromFirst = propVal - lowFrameVal;
            if (distFromFirst <= 0) {
                pctLow = 1;
                return;
            }
            float totDist = (*frameIter).mPropVal - lowFrameVal;
            if (!totDist) {
                pctLow = 0;
                return;
            }
            pctLow = 1.0f - distFromFirst / totDist;
            return;
        }
        lowFrame = (*frameIter).mPos;
        lowFrameVal = (*frameIter).mPropVal;
    }
    highFrame = mFrames[mFrames.size()-1].mPos;
    pctLow = 0;
}
