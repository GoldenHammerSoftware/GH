// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include <vector>
#include <stddef.h>

class GHIdentifier;
class GHGUIWidget;

// class to wrap a list of widgets that are sorted by priority then insertion order.
class GHGUIChildList
{
public:
	void add(GHResourcePtr<GHGUIWidget>* res, float priority);
	bool remove(GHResourcePtr<GHGUIWidget>* res);
	GHResourcePtr<GHGUIWidget>* find(const GHIdentifier& widgetId);
	float findPriority(const GHIdentifier& widgetId);
	const GHResourcePtr<GHGUIWidget>* find(const GHIdentifier& widgetId) const;
	bool contains(GHResourcePtr<GHGUIWidget>* res);

	size_t size(void) const { return mChildren.size(); }
	GHResourcePtr<GHGUIWidget>* operator [](size_t idx) { return mChildren[idx].mChild; }
	const GHResourcePtr<GHGUIWidget>* operator[](size_t idx) const { return mChildren[idx].mChild; }
    
    void clear(void) { mChildren.clear(); }

private:
	// we sort children by priority then insertion order.
	struct ChildEntry
	{
		GHResourcePtr<GHGUIWidget>* mChild;
		float mPriority;
		int mInsertionOrder;
	};
	struct ChildSorter
	{
		bool operator()(const ChildEntry& first, const ChildEntry second) const;
	};
    std::vector<ChildEntry> mChildren;
};
