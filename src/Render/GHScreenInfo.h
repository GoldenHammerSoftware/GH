// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHPlatform/GHDebugMessage.h"

// contains info about the screen or window that contains us.
class GHScreenInfo
{
public:
    GHScreenInfo(void)
		: mPixelSizeMult(1/800.0f, 0.0f)
        , mMaxPixelSize(-1)
        , mMinimumPixelHeight(0)
    {
        setSize(GHPoint2i(800,600));
		mPixelSize = calcPixelSize();
    }
    
    const GHPoint2i& getSize(void) const { return mSize; }
    float getAspectRatio(void) const { return mAspectRatio; }
    
	float getPixelSize(void) const { return mPixelSize; }
	float getSafeAreaPixelSize(const GHPoint2& safeArea) const
	{
		return calcPixelSize(safeArea[0], safeArea[1]);
	}

    void setSize(const GHPoint2i& size)
    { 
        mSize = size;
        mAspectRatio = ((float)size[0]) / (float)size[1];
        updateMinimumPixelHeight();
		mPixelSize = calcPixelSize();

        //GHDebugMessage::outputString("New aspect ratio %f", mAspectRatio);
    }
    void setPixelSizeFactor(float widthMult, float heightMult)
	{ 
		mPixelSizeMult[0] = widthMult;
		mPixelSizeMult[1] = heightMult;
		mPixelSize = calcPixelSize();
	}
    // set the minimum number of vpixels we will allow.
    // this is enforced by overriding max pixel size.
    void setMinimumPixelHeight(int val)
    {
        mMinimumPixelHeight = val;
        updateMinimumPixelHeight();
        mPixelSize = calcPixelSize();
    }
    
private:
	float calcPixelSize(float safeWidth = 1.0f, float safeHeight = 1.0f) const
	{
		float pixelSize = mSize[0]*mPixelSizeMult[0]*safeWidth + mSize[1]*mPixelSizeMult[1]*safeHeight;
        if (mMaxPixelSize > 0 && pixelSize > mMaxPixelSize) {
			pixelSize = mMaxPixelSize;
        }
		return pixelSize;
	}
    void updateMinimumPixelHeight(void)
    {
        if (mMinimumPixelHeight == 0) {
            mMaxPixelSize = -1;
            return;
        }
        
        float pixelPct = ((float)mSize[1]) / (float)mMinimumPixelHeight;
        mMaxPixelSize = pixelPct;
    }
    
private:
    // size in pixels.
    GHPoint2i mSize;
	// val we use to calculate pixel size.
	GHPoint2 mPixelSizeMult;
    // virtual pixel size, used to scale FT_PIXEL sizes.
    // gets bigger or smaller based on width or height, but not both.
    float mPixelSize;
    // a limit value for how large in real pixels a virtual pixel can be.
    // if <0 then no limit exists.
    float mMaxPixelSize;
    // a limit to the minimum number of vertical pixels we guarantee will fit within screen size.
    int mMinimumPixelHeight;
    // scale between width/height
    float mAspectRatio;
};
