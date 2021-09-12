// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHRect.h"
#include "Render/GHRenderable.h"

class GHRenderPass;
class GHGUIWidget;

// something that will draw a representation of a gui object
class GHGUIWidgetRenderer : public GHRenderable
{
public:
	GHGUIWidgetRenderer(void) {}
	GHGUIWidgetRenderer(float drawLayer, float drawOrderOffset)
		: mDrawLayer(drawLayer)
		, mDrawOrderOffset(drawOrderOffset)
	{}
    virtual ~GHGUIWidgetRenderer(void) {}

	virtual void updateRect(const GHRect<float, 2> & rect, float angleRadians) = 0;

	void setWidget(GHGUIWidget* widget) { mWidget = widget; }
	const GHGUIWidget* getWidget(void) const { return mWidget; }

	float getDrawLayer(void) const { return mDrawLayer; }

	virtual void updateDrawOrder(float baseGUIDrawOrder, float offset)
	{
		if (mDrawLayer != 0) mDrawOrder = baseGUIDrawOrder + mDrawLayer;
		mDrawOrder = baseGUIDrawOrder + offset + mDrawOrderOffset;
	}

protected:
	GHGUIWidget* mWidget{ 0 };
	// A value we use to batch widgets together for better material batching.
	// This is an optimization tool for use when we understand the drawing of a specific gui and want to minimize state switches.
	// It is not always viable to use due to possible drawing order overlap issues.
	// We will first draw objects on layer 0, then layer 1, etc.
	float mDrawLayer{ 0 };
	// A value we add to any incoming drawOrder for fudging.
	// This is a tool for fixing drawing order issues that are tricky and dont fit within the usual tools.
	float mDrawOrderOffset{ 0 };
};
