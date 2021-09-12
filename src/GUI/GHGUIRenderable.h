// Copyright Golden Hammer Software
#pragma once

#include "Render/GHRenderable.h"
#include <vector>
#include "Render/GHRenderPassMembership.h"
#include "GHMath/GHPoint.h"
#include <map>

class GHGUIWidgetRenderer;
class IGHGUICanvas;

class GHGUIRenderable : public GHRenderable
{
public:
    GHGUIRenderable(void);
    
    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
    
    void addRenderable(GHGUIWidgetRenderer& widget);
    void removeRenderable(GHGUIWidgetRenderer& widget);

    GHRenderPassMembership& getPassMembership(void) { return mPassMembership; }
    
	void toggleEnabled(void) { mEnabled = !mEnabled; }

private:
	struct WidgetListEntry
	{
		GHGUIWidgetRenderer* mWidget{ 0 };
		IGHGUICanvas* mCanvas{ 0 };
	};
    std::vector<WidgetListEntry> mWidgets;
    GHRenderPassMembership mPassMembership;

	class ListSorter
	{
	public:
		ListSorter(const std::map<IGHGUICanvas*, float>& depths);

		bool operator()(const GHGUIRenderable::WidgetListEntry& first, const GHGUIRenderable::WidgetListEntry& second) const;

	private:
		const std::map<IGHGUICanvas*, float>& mDepths;
	};

	// a way to turn off the gui rendering for screenshots or whatever
	bool mEnabled{ true };
};
