// Copyright 2010 Golden Hammer Software
#include "GTPosDescConverter.h"
#include "GHGUIRectMaker.h"
#include "GHMath/GHFloat.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHScreenInfo.h"
#include "GHGUIText.h"
#include "GHGUICanvas.h"

GTPosDescConverter::GTPosDescConverter(const GHGUIRectMaker& rectMaker, const GHScreenInfo& screenInfo)
	: mRectMaker(rectMaker)
	, mScreenInfo(screenInfo)
{

}

float GTPosDescConverter::convertLocalCoordToPCT(const GHGUIWidget& widget, float coord, int index) const
{
	GHFillType::Enum fillType = index == 0 ? widget.getPosDesc().mXFill : widget.getPosDesc().mYFill;
	GHGUICanvas* canvas = nullptr;
	return mRectMaker.createScreenPct(coord, fillType, index, widget.getParent(), widget.calcPixelSizeMod(), canvas);
}

float GTPosDescConverter::convertPCTToLocalCoord(const GHGUIWidget& widget, float pct, int index) const
{
	GHFillType::Enum fillType = index == 0 ? widget.getPosDesc().mXFill : widget.getPosDesc().mYFill;
	GHGUICanvas* canvas = nullptr;
	return mRectMaker.createLocalCoord(pct, fillType, index, widget.getParent(), widget.calcPixelSizeMod(), canvas);
}

float GTPosDescConverter::convertPCTToLocalPCT(const GHGUIWidget& widget, float pct, int index) const
{
	const GHRect<float, 2>* parent = widget.getParent();

	if (parent)
	{
		float parentSize = parent->mMax[index];
		parentSize -= parent->mMin[index];

		return pct / parentSize;
	}
	else {
		return pct;
	}
}

float GTPosDescConverter::convertLocalCoordToLocalPCT(const GHGUIWidget& widget, float coord, int index) const
{
	float globalPct = convertLocalCoordToPCT(widget, coord, index);
	return convertPCTToLocalPCT(widget, globalPct, index);
}

float GTPosDescConverter::convertLocalPCTToLocalCoord(const GHGUIWidget& widget, float pct, int index) const
{
	float globalPct = pct;
	const GHRect<float, 2>* parent = widget.getParent();
	if (parent)
	{
		float parentSize = parent->mMax[index];
		parentSize -= parent->mMin[index];

		float localPct = pct;
		globalPct = localPct * parentSize;
		globalPct += parent->mMin[index];
	}
	return convertPCTToLocalCoord(widget, globalPct, index);
}

float GTPosDescConverter::convertLocalCoordToPixel(const GHGUIWidget& widget, float coord, int index) const
{
	float pct = convertLocalCoordToPCT(widget, coord, index);
	return (pct / mScreenInfo.getPixelSize()) * (float)mScreenInfo.getSize()[index];
}

float GTPosDescConverter::convertPixelToLocalCoord(const GHGUIWidget& widget, float coord, int index) const
{
	float pct = (coord / (float)mScreenInfo.getSize()[index]) * mScreenInfo.getPixelSize();
	return convertPCTToLocalCoord(widget, coord, index);
}

void GTPosDescConverter::convertLocalCoordToPCT(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertLocalCoordToPCT(widget, point[0], 0);
	point[1] = convertLocalCoordToPCT(widget, point[1], 1);
}

void GTPosDescConverter::convertPCTToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertPCTToLocalCoord(widget, point[0], 0);
	point[1] = convertPCTToLocalCoord(widget, point[1], 1);
}

void GTPosDescConverter::convertLocalCoordToLocalPCT(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertLocalCoordToLocalPCT(widget, point[0], 0);
	point[1] = convertLocalCoordToLocalPCT(widget, point[1], 1);
}

void GTPosDescConverter::convertLocalPCTToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertLocalPCTToLocalCoord(widget, point[0], 0);
	point[1] = convertLocalPCTToLocalCoord(widget, point[1], 1);
}

void GTPosDescConverter::convertLocalCoordToPCT(const GHGUIWidget& widget, GHGUIPosDesc& posDesc) const
{
	convertLocalCoordToPCT(widget, posDesc.mOffset);
	convertLocalCoordToPCT(widget, posDesc.mSize);
}

void GTPosDescConverter::convertPCTToLocalCoord(const GHGUIWidget& widget, GHGUIPosDesc& posDesc) const
{
	convertPCTToLocalCoord(widget, posDesc.mOffset);
	convertPCTToLocalCoord(widget, posDesc.mSize);
}

void GTPosDescConverter::convertLocalCoordToPixel(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertLocalCoordToPixel(widget, point[0], 0);
	point[1] = convertLocalCoordToPixel(widget, point[1], 1);
}

void GTPosDescConverter::convertPixelToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const
{
	point[0] = convertPixelToLocalCoord(widget, point[0], 0);
	point[1] = convertPixelToLocalCoord(widget, point[1], 1);
}

void GTPosDescConverter::changeParent(GHGUIWidgetResource& widget, GHGUIWidgetResource* oldParent, GHGUIWidgetResource& newParent)
{
	widget.acquire();
	GHGUIPosDesc posDesc = widget.get()->getPosDesc();
	convertLocalCoordToPCT(*widget.get(), posDesc);

	GHRect<float, 2> screenRect = widget.get()->getScreenPos();

	//uncalculate align
	GHPoint2 unAlignedMin;
	calcUnalignedMin(*widget.get(), posDesc.mAlign, unAlignedMin);

	if (oldParent) {
		oldParent->get()->removeChild(&widget);
	}
	newParent.get()->addChild(&widget);

	//recalculate align
	{
		const GHRect<float, 2>* newParentRectPtr = widget.get()->getParent();
		GHRect<float, 2> newParentRect(GHPoint2(0, 0), GHPoint2(1, 1));
		if (newParentRectPtr) {
			newParentRect = *newParentRectPtr;
		}
		GHPoint2 newParentSize = newParentRect.mMax;
		newParentSize -= newParentRect.mMin;

		//mAlign = -(unAlignedMin + parent.mRect.mMin - oldRect.mMin) / parentSize
		posDesc.mAlign = newParentRect.mMin;
		posDesc.mAlign += unAlignedMin;
		posDesc.mAlign -= screenRect.mMin;
		posDesc.mAlign[0] /= newParentSize[0];
		posDesc.mAlign[1] /= newParentSize[1];
		posDesc.mAlign *= -1.f;
	}
	//GHGUIPosDesc alignChangedPosDesc = widget.get()->getPosDesc();
	//alignChangedPosDesc.mAlign = posDesc.mAlign;
	//widget.get()->setPosDesc(alignChangedPosDesc);

	convertPCTToLocalCoord(*widget.get(), posDesc);
	widget.get()->setPosDesc(posDesc);
	widget.release();

	widget.get()->updatePosition();
}

void GTPosDescConverter::calcUnalignedMin(const GHGUIWidget& widget, const GHPoint2& align, GHPoint2& unAlignedMin) const
{
	const GHRect<float, 2>& screenRect = widget.getScreenPos();
	const GHRect<float, 2>* oldParentRectPtr = widget.getParent();
	GHRect<float, 2> oldParentRect(GHPoint2(0, 0), GHPoint2(1, 1));
	if (oldParentRectPtr) {
		oldParentRect = *oldParentRectPtr;
	}

	GHPoint2 oldParentSize = oldParentRect.mMax;
	oldParentSize -= oldParentRect.mMin;

	//screenRect.mMin = unAlignedMin + oldParent.mRect.mMin + oldParentSize * mAlign;
	//unAlignedMin = -(oldParent.mRect.mMin + oldParentSize*mAlign - screenRect.mMin)
	unAlignedMin = oldParentSize;
	unAlignedMin[0] *= align[0];
	unAlignedMin[1] *= align[1];
	unAlignedMin -= screenRect.mMin;
	unAlignedMin += oldParentRect.mMin;
	unAlignedMin *= -1.f;
}

void GTPosDescConverter::setSize(GHGUIWidget& widget, const GHPoint2& size) const
{

}

void GTPosDescConverter::setAlign(GHGUIWidget& widget, const GHPoint2& align) const
{
	GHGUIPosDesc posDesc = widget.getPosDesc();
	//GHPoint2 unalignedMin;
	//calcUnalignedMin(widget, posDesc.mAlign, unalignedMin);

	//GHPoint2 offsetUnAlignedMin;
	//calcUnalignedMin(widget, align, offsetUnAlignedMin);

	GHPoint2 offset = posDesc.mOffset;
	convertLocalCoordToLocalPCT(widget, offset);

	GHPoint2 alignDiff = align;
	alignDiff -= posDesc.mAlign;
	offset -= alignDiff;

	convertLocalPCTToLocalCoord(widget, offset);

	posDesc.mAlign = align;
	posDesc.mOffset = offset;

	widget.setPosDesc(posDesc);
	widget.updatePosition();
}

void GTPosDescConverter::setOffset(GHGUIWidget& widget, const GHPoint2& offset) const
{

}

void GTPosDescConverter::setXFill(GHGUIWidget& widget, GHFillType::Enum xFill) const
{
	setFill(widget, xFill, 0);
}

void GTPosDescConverter::setYFill(GHGUIWidget& widget, GHFillType::Enum yFill) const
{
	setFill(widget, yFill, 1);
}

void GTPosDescConverter::setFill(GHGUIWidget& widget, GHFillType::Enum newFill, int index) const
{
	GHGUIPosDesc posDesc = widget.getPosDesc();
	GHFillType::Enum& targetFill = index == 0 ? posDesc.mXFill : posDesc.mYFill;
	GHFillType::Enum prevFill = targetFill;
	if (newFill == prevFill) { return; }

	float sizeInPct = convertLocalCoordToPCT(widget, widget.getPosDesc().mSize[index], index);
	float offsetInPct = convertLocalCoordToPCT(widget, widget.getPosDesc().mOffset[index], index);
	
	targetFill = newFill;

	widget.setPosDesc(posDesc);

	posDesc.mSize[index] = convertPCTToLocalCoord(widget, sizeInPct, index);
	posDesc.mOffset[index] = convertPCTToLocalCoord(widget, offsetInPct, index);

	widget.setPosDesc(posDesc);
}

void GTPosDescConverter::setTextHeightFillType(GHGUIText& text, GHFillType::Enum fill) const
{
	//text height is always wrt vertical size
	int heightIndex = 1;
	
	float oldHeight = text.getTextHeight();

	GHFillType::Enum oldFillType = text.getHeightFillType();
	GHGUICanvas* canvas = nullptr;
	float heightInPct = mRectMaker.createScreenPct(oldHeight, oldFillType, heightIndex, text.getParent(), text.calcPixelSizeMod(), canvas);

	text.setHeightFillType(fill);

	float newHeight = mRectMaker.createLocalCoord(heightInPct, fill, heightIndex, text.getParent(), text.calcPixelSizeMod(), canvas);
	text.setTextHeight(newHeight);
}

void GTPosDescConverter::setXSizeAlign(GHGUIWidget& widget, GHAlign::Enum align) const
{
	//the rest of the position says where a point belongs.
	//xSizeAlign = center means the box is centered on that position.
	//left means the point is on the right edge of the box

	GHGUIPosDesc desc = widget.getPosDesc();

	GHAlign::Enum prevAlign = desc.mSizeAlign[0];

	if (align == prevAlign) { return; }

	if (prevAlign == GHAlign::A_CENTER)
	{
		if (align == GHAlign::A_LEFT)
		{
			desc.mOffset[0] += desc.mSize[0] * .5f;
		}
		else if (align = GHAlign::A_RIGHT)
		{
			desc.mOffset[0] -= desc.mSize[0] * .5f;
		}
	}
	else if (prevAlign == GHAlign::A_LEFT)
	{
		if (align = GHAlign::A_CENTER)
		{
			desc.mOffset[0] -= desc.mSize[0] * .5f;
		}
		else if (align = GHAlign::A_RIGHT)
		{
			desc.mOffset[0] -= desc.mSize[0];
		}
	}
	else if (prevAlign == GHAlign::A_RIGHT)
	{
		if (align = GHAlign::A_CENTER)
		{
			desc.mOffset[0] += desc.mSize[0] * .5f;
		}
		else if (align = GHAlign::A_LEFT)
		{
			desc.mOffset[0] += desc.mSize[0];
		}
	}
	

	desc.mSizeAlign[0] = align;
	widget.setPosDesc(desc);
}

void GTPosDescConverter::setYSizeAlign(GHGUIWidget& widget, GHAlign::Enum align) const
{
	GHGUIPosDesc desc = widget.getPosDesc();

	GHAlign::Enum prevAlign = desc.mSizeAlign[1];

	if (align == prevAlign) { return; }

	if (prevAlign == GHAlign::A_CENTER)
	{
		if (align == GHAlign::A_TOP)
		{
			desc.mOffset[1] += desc.mSize[1] * .5f;
		}
		else if (align = GHAlign::A_BOTTOM)
		{
			desc.mOffset[1] -= desc.mSize[1] * .5f;
		}
	}
	else if (prevAlign == GHAlign::A_TOP)
	{
		if (align = GHAlign::A_CENTER)
		{
			desc.mOffset[1] -= desc.mSize[1] * .5f;
		}
		else if (align = GHAlign::A_BOTTOM)
		{
			desc.mOffset[1] -= desc.mSize[1];
		}
	}
	else if (prevAlign == GHAlign::A_BOTTOM)
	{
		if (align = GHAlign::A_CENTER)
		{
			desc.mOffset[1] += desc.mSize[1] * .5f;
		}
		else if (align = GHAlign::A_TOP)
		{
			desc.mOffset[1] += desc.mSize[1];
		}
	}


	desc.mSizeAlign[1] = align;
	widget.setPosDesc(desc);
}
