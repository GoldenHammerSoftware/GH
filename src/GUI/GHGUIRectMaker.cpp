// Copyright Golden Hammer Software
#include "GHGUIRectMaker.h"
#include "GHGUIPosDesc.h"
#include "GHScreenInfo.h"
#include "GHGUICanvas.h"

GHGUIRectMaker::GHGUIRectMaker(const GHScreenInfo& screenInfo)
: mScreenInfo(screenInfo)
, mSafeAreaSize(1.0f, 1.0f)
{
}

static float calcNormalizedScreenPos(GHFillType::Enum fillType, float inSize, float screenSize, float pixelSize)
{
    float ret = 0;
    if (fillType == GHFillType::FT_PIXELS)
    {
        ret = inSize * pixelSize;
        ret /= screenSize;
    }
    else
    {
        ret = inSize;
    }
    return ret;
}

static void offsetScreenPos(const GHPoint2& align,
                            const GHPoint2& pos,
                            const GHPoint2& parentMin,
                            const GHPoint2& parentMax,
                            GHFillType::Enum xFill,
                            GHFillType::Enum yFill,
                            GHPoint2& ret)
{
    GHPoint2 parentSize = parentMax;
    parentSize -= parentMin;
    ret = pos;
    if (xFill == GHFillType::FT_PCT) ret[0] *= parentSize[0];
    if (yFill == GHFillType::FT_PCT) ret[1] *= parentSize[1];
    parentSize *= align;
    ret += parentSize;
    ret += parentMin;
}

GHGUIRectMaker::CanvasInfo GHGUIRectMaker::calcCanvasInfo(const IGHGUICanvas* canvas, float pixelSizeMod) const
{
	GHGUIRectMaker::CanvasInfo ret;
	if (!canvas || canvas->is2d())
	{
		// todo: add the ability to use a 2d canvas to draw to part of the screen.
		ret.mPixelSize = mScreenInfo.getSafeAreaPixelSize(mSafeAreaSize);
		ret.mCanvasSize = mScreenInfo.getSize();
	}
	else
	{
		// act as if width is always some arbitrary good value.
		const float kDefaultWindowWidth = 1024.0f;
		ret.mPixelSize = 1.0f;
		const GHPoint3& canvasScale = canvas->getScale();
		float aspectRatio = canvasScale[1] / canvasScale[0];
		ret.mCanvasSize[0] = (int)kDefaultWindowWidth;
		ret.mCanvasSize[1] = (int)(kDefaultWindowWidth * aspectRatio);
	}
	ret.mPixelSize *= pixelSizeMod;
	return ret;
}

void GHGUIRectMaker::createRect(const GHGUIPosDesc& desc, 
                                GHRect<float,2>& ret,
                                const GHRect<float,2>* parent,
                                float pixelSizeMod, 
								const IGHGUICanvas* canvas) const
{
    GHPoint2 sizeInPct;

	GHGUIRectMaker::CanvasInfo canvasInfo = calcCanvasInfo(canvas, pixelSizeMod);

	sizeInPct[0] = calcNormalizedScreenPos(desc.mXFill, desc.mSize[0], (float)canvasInfo.mCanvasSize[0],
		canvasInfo.mPixelSize);
	sizeInPct[1] = calcNormalizedScreenPos(desc.mYFill, desc.mSize[1], (float)canvasInfo.mCanvasSize[1],
		canvasInfo.mPixelSize);

	ret.mMin[0] = calcNormalizedScreenPos(desc.mXFill, desc.mOffset[0], (float)canvasInfo.mCanvasSize[0],
		canvasInfo.mPixelSize);
	ret.mMin[1] = calcNormalizedScreenPos(desc.mYFill, desc.mOffset[1], (float)canvasInfo.mCanvasSize[1],
		canvasInfo.mPixelSize);

    if (parent) {
        offsetScreenPos(desc.mAlign, ret.mMin, parent->mMin, parent->mMax, desc.mXFill, desc.mYFill, ret.mMin);
        if (desc.mXFill == GHFillType::FT_PCT) {
            sizeInPct[0] *= parent->mMax[0] - parent->mMin[0];
        }
        if (desc.mYFill == GHFillType::FT_PCT) {
            sizeInPct[1] *= parent->mMax[1] - parent->mMin[1];
        }
    }
    else {
        GHPoint2 parentMin(0,0);
        GHPoint2 parentMax(1,1);
        offsetScreenPos(desc.mAlign, ret.mMin, parentMin, parentMax, desc.mXFill, desc.mYFill, ret.mMin);
    }

    ret.mMax = ret.mMin;
    ret.mMax += sizeInPct;
    
    offsetSizeAlign(desc, sizeInPct, ret);
}

void GHGUIRectMaker::offsetSizeAlign(const GHGUIPosDesc& desc, const GHPoint2& sizeInPct,
                                     GHRect<float,2>& ret) const
{
    GHPoint2 offset(0,0);
    if (desc.mSizeAlign[0] == GHAlign::A_CENTER)
    {
        offset[0] -= sizeInPct[0]/2.0f;
    }
    else if (desc.mSizeAlign[0] == GHAlign::A_LEFT)
    {
        offset[0] -= sizeInPct[0];
    }
    
    if (desc.mSizeAlign[1] == GHAlign::A_CENTER)
    {
        offset[1] -= sizeInPct[1]/2.0f;
    }
    else if (desc.mSizeAlign[1] == GHAlign::A_TOP)
    {
        offset[1] -= sizeInPct[1];
    }
    ret.mMin += offset;
    ret.mMax += offset;
}

float GHGUIRectMaker::createScreenPct(const float& size, GHFillType::Enum fillType, int index,
	const GHRect<float,2>* parent, float pixelSizeMod, const IGHGUICanvas* canvas) const
{
	GHGUIRectMaker::CanvasInfo canvasInfo = calcCanvasInfo(canvas, pixelSizeMod);

    float ret = calcNormalizedScreenPos(fillType, size, (float)canvasInfo.mCanvasSize[index], canvasInfo.mPixelSize);
    if (parent && fillType != GHFillType::FT_PIXELS)
    {
        float parentSize = parent->mMax[index] - parent->mMin[index];
        ret *= parentSize;
    }
    return ret;
}

float GHGUIRectMaker::createLocalCoord(const float& size, GHFillType::Enum fillType, int index,
									  const GHRect<float, 2>* parent, float pixelSizeMod, const IGHGUICanvas* canvas) const
{
	if (fillType == GHFillType::FT_PIXELS)
	{
		GHGUIRectMaker::CanvasInfo canvasInfo = calcCanvasInfo(canvas, pixelSizeMod);
		return (size / canvasInfo.mPixelSize) * (float)canvasInfo.mCanvasSize[index];
	}

	if (parent)
	{
		float parentSize = parent->mMax[index] - parent->mMin[index];
		return size / parentSize;
	}
	return size;
}
