// Copyright Golden Hammer Software
#pragma once

#include <vector>
#include <algorithm>
#include "GHDebugMessage.h"
#include <stddef.h> //for size_t

template< typename T >
class GHPrioritizedListDefaultCallback
{
public:
    void onAdd(const T& ) { }
    void onRemove(const T& ) { }
};

// A way to store things in a list that gets sorted by priority.
// Also handles adding/removing while updating.
template< typename T, typename Callback = GHPrioritizedListDefaultCallback<T> >
class GHPrioritizedList
{
private:
	class ListEntry
	{
	public:
		ListEntry(const T& thing, float pri)
		: mThing(thing)
		, mPriority(pri)
		{}
		
		T mThing;
		float mPriority;
	};
	
    // type of list change the we queue in a ListChange
    enum ChangeType
    {
        CT_ADD,
        CT_REMOVE,
        // for disabling a change that is in the list without removing it from the list.
        CT_DONOTHING
    };
    
	class ListChange
	{
	public:
		ListChange(bool add, const T& thing, float pri)
		: mChangeType(CT_DONOTHING)
		, mThing(thing)
		, mPriority(pri)
		{
            if (add) mChangeType = CT_ADD;
            else mChangeType = CT_REMOVE;
        }
		
		ChangeType mChangeType;
		T mThing;
		float mPriority;
	};
	
	class ListSorter
	{
	public:
		bool operator()(const ListEntry& first, const ListEntry& second) const
		{
			if (first.mPriority != second.mPriority)
			{
				return first.mPriority < second.mPriority;
			}
			return first.mThing < second.mThing;
		}
	};
	
public:
	GHPrioritizedList(bool listUsesSort=true, const Callback& callback=Callback())
	: mIsIterating(false)
	, mSortNeeded(false)
    , mCallback(callback)
	, mListUsesSort(listUsesSort)
	{}
	
	// tell the list that we are iterating.
	// prevents adding/removing mid-iterate from causing problems.
	// start also sorts the list if necessary.
	void startIteration(void)
	{
		mIsIterating = true;
		if (mSortNeeded) sort();
	}
	
	void endIteration(void)
	{
        while (mListChanges.size())
        {
            ChangeList listCopy = mListChanges;
            mListChanges.clear();
            
            for (size_t i = 0; i < listCopy.size(); ++i)
            {
                if (listCopy[i].mChangeType == CT_ADD) {
                    addNow(listCopy[i].mThing, listCopy[i].mPriority);
                }
                else if (listCopy[i].mChangeType == CT_REMOVE) {
                    removeNow(listCopy[i].mThing);
                }
            }
            mSortNeeded = true;
        }
		mIsIterating = false;
	}
	
	void addEntry(const T& val, float pri)
	{
		mSortNeeded = true;
		if (!mIsIterating)
		{
			addNow(val, pri);
		}
		else 
		{
			mListChanges.push_back(ListChange(true, val, pri));
		}
	}
	
    void updatePriority(const T& val, float pri)
    {
        mSortNeeded = true;
        typename GHPrioritizedList<T, Callback>::EntryList::iterator findIter;
        for (findIter = mList.begin(); findIter != mList.end(); ++findIter)
        {
            if ((*findIter).mThing == val)
            {
                (*findIter).mPriority = pri;
                return;
            }
        }
    }
                
	void removeEntry(const T& val)
	{
		if (!mIsIterating)
		{
			removeNow(val);
		}
		else 
		{
			mListChanges.push_back(ListChange(false, val, 0));
		}
	}
	
	size_t getNumEntries(void) const { return mList.size(); }
	T getEntry(size_t index) const { return mList[index].mThing; }
	
    bool isInChangeList(const T& val) const
    {
        for (size_t i = 0; i < mListChanges.size(); ++i)
        {
            if (mListChanges[i].mThing == val) return true;
        }
        return false;
    }
    
private:
	void sort(void)
	{
		mSortNeeded = false;
		if (mListUsesSort)
		{
			std::sort(mList.begin(), mList.end(), ListSorter());
		}
	}
    
    void addNow(const T& val, float pri)
    {
#define TEST_DOUBLE_ADD
#ifdef TEST_DOUBLE_ADD
        typename GHPrioritizedList<T, Callback>::EntryList::iterator findIter;
        for (findIter = mList.begin(); findIter != mList.end(); ++findIter)
        {
            if ((*findIter).mThing == val)
            {
                GHDebugMessage::outputString("Adding a T twice in addNow");
            }
        }
#endif
        mList.push_back(ListEntry(val, pri));
        mCallback.onAdd(val);
    }
    
    void removeNow(const T& val)
    {
        typename GHPrioritizedList<T, Callback>::EntryList::iterator findIter;
        for (findIter = mList.begin(); findIter != mList.end(); ++findIter)
        {
            if ((*findIter).mThing == val)
            {
                mCallback.onRemove(val);
                mList.erase(findIter);
                return;
            }
        }
        GHDebugMessage::outputString("Failed to find T for removeNow");
    }
	
private:
	bool mIsIterating;
	// does this list sort or leave things in original order?
	bool mListUsesSort;
	// has our list become invalidated causing a need for a sort?
	bool mSortNeeded;
	typedef std::vector< ListEntry > EntryList;
	typedef std::vector< ListChange > ChangeList;
	EntryList mList;
	ChangeList mListChanges;	
    Callback mCallback;
};
