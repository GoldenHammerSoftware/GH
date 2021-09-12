// Copyright Golden Hammer Software
#include "GHGUIChildList.h"
#include <algorithm>
#include "GHGUIWidget.h"

void GHGUIChildList::add(GHGUIWidgetResource* res, float priority)
{
	// find the highest insertion order value for the given priority;
	int insertionOrder = 0;
    std::vector<ChildEntry>::iterator findIter;
    for (findIter = mChildren.begin(); findIter != mChildren.end(); ++findIter)
	{
		if ((*findIter).mPriority > priority) break;
		if ((*findIter).mPriority == priority) {
			if ((*findIter).mInsertionOrder <= insertionOrder) {
				insertionOrder = (*findIter).mInsertionOrder + 1;
			}
		}
	}

	ChildEntry entry;
	entry.mChild = res;
	entry.mPriority = priority;
	entry.mInsertionOrder = insertionOrder;

    mChildren.push_back(entry);
	std::sort(mChildren.begin(), mChildren.end(), ChildSorter());
}

bool GHGUIChildList::remove(GHGUIWidgetResource* res)
{
    std::vector<ChildEntry>::iterator findIter;
    for (findIter = mChildren.begin(); findIter != mChildren.end(); ++findIter)
    {
        if ((*findIter).mChild == res) 
		{
            mChildren.erase(findIter);
            return true;
        }
    }
	return false;
}

GHGUIWidgetResource* GHGUIChildList::find(const GHIdentifier& widgetId)
{
	//using const_cast here to avoid duplication. the real code is in the const version
	// to guarantee that the find logic is in fact const.
	return const_cast<GHGUIWidgetResource*>(const_cast<const GHGUIChildList*>(this)->find(widgetId));
}

float GHGUIChildList::findPriority(const GHIdentifier& widgetId)
{
	std::vector<ChildEntry>::const_iterator findIter;
	for (findIter = mChildren.begin(); findIter != mChildren.end(); ++findIter)
	{
		if ((*findIter).mChild->get()->getId() == widgetId ||
			(*findIter).mChild->getResourceId() == widgetId)
		{
			return (*findIter).mPriority;
		}
	}
	return 0;
}

const GHGUIWidgetResource* GHGUIChildList::find(const GHIdentifier& widgetId) const
{
	std::vector<ChildEntry>::const_iterator findIter;
	for (findIter = mChildren.begin(); findIter != mChildren.end(); ++findIter)
	{
		if ((*findIter).mChild->get()->getId() == widgetId ||
			(*findIter).mChild->getResourceId() == widgetId)
		{
			return (*findIter).mChild;
		}

		GHGUIWidgetResource* test = (*findIter).mChild->get()->getChild(widgetId);
		if (test) return test;
	}
	return 0;
}

bool GHGUIChildList::contains(GHGUIWidgetResource* res)
{
    std::vector<ChildEntry>::iterator findIter;
    for (findIter = mChildren.begin(); findIter != mChildren.end(); ++findIter)
    {
        if ((*findIter).mChild == res) return true;
    }    
    return false;
}

bool GHGUIChildList::ChildSorter::operator()(const ChildEntry& first, const ChildEntry second) const
{
	if (first.mPriority != second.mPriority) return first.mPriority < second.mPriority;
	if (first.mInsertionOrder != second.mInsertionOrder) return first.mInsertionOrder < second.mInsertionOrder;
	return first.mChild < second.mChild;
}
