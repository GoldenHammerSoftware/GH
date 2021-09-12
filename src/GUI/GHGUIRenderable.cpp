// Copyright Golden Hammer Software
#include "GHGUIRenderable.h"
#include "GHRenderPass.h"
#include "GHGUIWidgetRenderer.h"
#include "GHPlatform/GHDebugMessage.h"
#include <algorithm>
#include "GHGUIWidget.h"
#include "GHGUICanvas.h"
#include "GHCamera.h"

GHGUIRenderable::GHGUIRenderable(void)
{
}

GHGUIRenderable::ListSorter::ListSorter(const std::map<IGHGUICanvas*, float>& depths)
	: mDepths(depths)
{
}

bool GHGUIRenderable::ListSorter::operator()(const GHGUIRenderable::WidgetListEntry& first, const GHGUIRenderable::WidgetListEntry& second) const
{
	// handle the odd case of not having a gui widget for a renderable.
    if (!first.mWidget->getWidget() || !second.mWidget->getWidget()) {
        return first.mWidget < second.mWidget;
    }
	// sort drawlayer objects together.  they will draw as one in order to maximize material batching.  drawLayer is an optimization option.
	if (first.mWidget->getDrawLayer() != second.mWidget->getDrawLayer())
	{
		return second.mWidget->getDrawLayer() > first.mWidget->getDrawLayer();
	}
	if (first.mCanvas != second.mCanvas)
	{
		// 2d canvases draw last
		if (!first.mCanvas || !second.mCanvas)
		{
			if (!first.mCanvas) return false;
			return true;
		}
		// if both have a 3d canvas sort by canvas depth.
		else
		{
			float firstDepth = mDepths.find(first.mCanvas)->second;
			float secondDepth = mDepths.find(second.mCanvas)->second;
			if (firstDepth != secondDepth)
			{
				return (secondDepth < firstDepth);
			}
		}
	}
	// within the same canvas/depth sort by widget depth order.
    int firstOrder = first.mWidget->getWidget()->getDepth();
    int secondOrder = second.mWidget->getWidget()->getDepth();
    if (firstOrder < secondOrder) return true;
    if (secondOrder < firstOrder) return false;
    return &first < &second;
}

void GHGUIRenderable::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
	if (!mEnabled)
	{
		return;
	}
    if (!mPassMembership.supportsPass(pass.getIdentifier())) 
    {
        return;
    }

	// calc canvas depths.
	// todo: prune out non-visible canvases.
	std::map<IGHGUICanvas*, float> depths;
	GHPoint3 camPos;
	pass.getCamera().getTransform().getTranslate(camPos);
	GHPoint3 depthPos;
	for (size_t i = 0; i < mWidgets.size(); ++i)
	{
		if (!mWidgets[i].mCanvas) continue;
		auto findIter = depths.find(mWidgets[i].mCanvas);
		if (findIter != depths.end()) continue;

		GHPoint3 depthPos = mWidgets[i].mCanvas->getPos();
		depthPos -= camPos;
		depths[mWidgets[i].mCanvas] = depthPos.lenSqr();
	}

	ListSorter sorter(depths);
	std::sort(mWidgets.begin(), mWidgets.end(), sorter);

    static const float drawOrderInc = 1.0f;
    float drawOrderOffset = 0;
	float drawLayer = 0;
	size_t numWidgetsInDrawLayer = 0;
	float layerDrawOrder = mDrawOrder;
    for (size_t i = 0; i < mWidgets.size(); ++i)
    {
		if (drawLayer != mWidgets[i].mWidget->getDrawLayer())
		{
			drawLayer = mWidgets[i].mWidget->getDrawLayer();
			layerDrawOrder += numWidgetsInDrawLayer;
			numWidgetsInDrawLayer = 0;
		}
        mWidgets[i].mWidget->updateDrawOrder(layerDrawOrder, drawOrderOffset);
        mWidgets[i].mWidget->collect(pass, frustum);
        drawOrderOffset += drawOrderInc;
		numWidgetsInDrawLayer++;
    }
}

void GHGUIRenderable::addRenderable(GHGUIWidgetRenderer& widget)
{
    // test code for not adding twice.
    for (auto widgeIter = mWidgets.begin(); widgeIter != mWidgets.end(); ++widgeIter)
    {
        if ((*widgeIter).mWidget == &widget) {
            GHDebugMessage::outputString("Adding a widget renderer twice!");
            return;
        }
    }
    
	WidgetListEntry entry;
	entry.mWidget = &widget;
	if (widget.getWidget())
	{
		IGHGUICanvas* canvas = GHGUICanvas::get3dCanvas(widget.getWidget()->getPropertyContainer());
		if (canvas)
		{
			entry.mCanvas = canvas;
		}
	}
    mWidgets.push_back(entry);
}

void GHGUIRenderable::removeRenderable(GHGUIWidgetRenderer& widget)
{
    for (auto widgeIter = mWidgets.begin(); widgeIter != mWidgets.end(); ++widgeIter)
    {
        if ((*widgeIter).mWidget == &widget) {
            mWidgets.erase(widgeIter);
            return;
        }
    }
    GHDebugMessage::outputString("Failed to find widget renderer for removal");
}
