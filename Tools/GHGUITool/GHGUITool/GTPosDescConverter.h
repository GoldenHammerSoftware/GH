// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHMath/GHPoint.h"
#include "GHUtils/GHAlign.h"
#include "GHFillType.h"
#include "GHGUIWidget.h"

class GHScreenInfo;
class GHGUIText;

class GTPosDescConverter
{
public:
	GTPosDescConverter(const GHGUIRectMaker& rectMaker, const GHScreenInfo& screenInfo);

	float convertLocalCoordToPCT(const GHGUIWidget& widget, float coord, int index) const;
	float convertPCTToLocalCoord(const GHGUIWidget& widget, float pct, int index) const;

	float convertPCTToLocalPCT(const GHGUIWidget& widget, float pct, int index) const;

	float convertLocalCoordToLocalPCT(const GHGUIWidget& widget, float coord, int index) const;
	float convertLocalPCTToLocalCoord(const GHGUIWidget& widget, float pct, int index) const;

	void convertLocalCoordToPCT(const GHGUIWidget& widget, GHPoint2& point) const;
	void convertPCTToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const;

	void convertLocalCoordToLocalPCT(const GHGUIWidget& widget, GHPoint2& point) const;
	void convertLocalPCTToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const;

	void convertLocalCoordToPCT(const GHGUIWidget& widget, GHGUIPosDesc& posDesc) const;
	void convertPCTToLocalCoord(const GHGUIWidget& widget, GHGUIPosDesc& posDesc) const;

	float convertLocalCoordToPixel(const GHGUIWidget& widget, float coord, int index) const;
	float convertPixelToLocalCoord(const GHGUIWidget& widget, float coord, int index) const;

	void convertLocalCoordToPixel(const GHGUIWidget& widget, GHPoint2& point) const;
	void convertPixelToLocalCoord(const GHGUIWidget& widget, GHPoint2& point) const;

	void changeParent(GHGUIWidgetResource& widget, GHGUIWidgetResource* oldParent, GHGUIWidgetResource& newParent);

	void setSize(GHGUIWidget& widget, const GHPoint2& size) const;
	void setAlign(GHGUIWidget& widget, const GHPoint2& align) const;
	void setOffset(GHGUIWidget& widget, const GHPoint2& offset) const;
	void setXFill(GHGUIWidget& widget, GHFillType::Enum xFill) const;
	void setYFill(GHGUIWidget& widget, GHFillType::Enum yFill) const;
	void setXSizeAlign(GHGUIWidget& widget, GHAlign::Enum align) const;
	void setYSizeAlign(GHGUIWidget& widget, GHAlign::Enum align) const;
	void setTextHeightFillType(GHGUIText& text, GHFillType::Enum fill) const;

private:
	void setFill(GHGUIWidget& widget, GHFillType::Enum fill, int index) const;
	void calcUnalignedMin(const GHGUIWidget& widget, const GHPoint2& align, GHPoint2& unalignedMin) const;

private:
	const GHGUIRectMaker& mRectMaker;
	const GHScreenInfo& mScreenInfo;
};
