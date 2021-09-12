// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include "GHFillType.h"
#include "GHUtils/GHAlign.h"

// A description of what part of the screen a gui item should take up.
// Used along with the screen info to generate the actual pos.
class GHGUIPosDesc
{
public:
 GHGUIPosDesc(void)
    : mXFill(GHFillType::FT_PCT)
    , mYFill(GHFillType::FT_PCT)
    , mAlign(0,0)
    , mOffset(0,0)
    , mSize(1,1)
    , mAngleRadians(0)
    {
        mSizeAlign[0] = GHAlign::A_RIGHT;
        mSizeAlign[1] = GHAlign::A_BOTTOM;
    }

 bool operator==(const GHGUIPosDesc& other) const
 {
	 return !(*this != other);
 }

 bool operator!=(const GHGUIPosDesc& other) const
 {
	 return memcmp(this, &other, sizeof(GHGUIPosDesc)) != 0;
 }
    
public:
    GHFillType::Enum mXFill;
    GHFillType::Enum mYFill;
    // normalized screen space position that mOffset is applied to.
    // align of 0.5 0.5 means the center of the screen.
    GHPoint2 mAlign;
    // Offset of the anchor point from left/center/right in x and y based on FillType
    GHPoint2 mOffset;
    // How much screen we take up based on FillType
    GHPoint2 mSize;
    // how our rect centers around the anchor point.
    // if left: the right edge of the box touches the anchor
    // if right: the left edge touches the anchor
    // if center: the left edge is center - size/2
    GHAlign::Enum mSizeAlign[2];
    float mAngleRadians;
};
