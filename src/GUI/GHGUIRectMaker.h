// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHRect.h"
#include "GHFillType.h"

class GHScreenInfo;
class GHGUIPosDesc;
class IGHGUICanvas;

// Widget class to take a pos description and turn it into a screen space rect
class GHGUIRectMaker
{
public:
    GHGUIRectMaker(const GHScreenInfo& screenInfo);
    
    void createRect(const GHGUIPosDesc& desc, GHRect<float,2>& ret, const GHRect<float,2>* parent, float pixelSizeMod, const IGHGUICanvas* canvas) const;
    
    // take a size in pixels or parent pct and turn it into a screen pct
    // index: 0 - x, 1 - y
    float createScreenPct(const float& size, GHFillType::Enum fillType, int index,
                          const GHRect<float,2>* parent, float pixelSizeMod, const IGHGUICanvas* canvas) const;

	// take a size in screen pct and turn into pixels or parent pct
	// index: 0 - x, 1 - y
	float createLocalCoord(const float& size, GHFillType::Enum fillType, int index,
						  const GHRect<float, 2>* parent, float pixelSizeMod, const IGHGUICanvas* canvas) const;
    
	// Let the rect maker know that there's a global modifier on the screen size.
	// Use this for keeping pixel size reasonable.
	void setSafeAreaPct(const GHPoint2& safeAreaSize) { mSafeAreaSize = safeAreaSize; }

public:
	struct CanvasInfo
	{
		float mPixelSize;
		GHPoint2i mCanvasSize;
	};
	CanvasInfo calcCanvasInfo(const IGHGUICanvas* canvas, float pixelSizeMod) const;

private:
    void offsetSizeAlign(const GHGUIPosDesc& desc, const GHPoint2& sizeInPct,
                         GHRect<float,2>& ret) const;
                   
private:
    const GHScreenInfo& mScreenInfo;
	GHPoint2 mSafeAreaSize;
};
