// Copyright Golden Hammer Software
#include "GHRenderListContainer.h"

#include <algorithm>
#include "GHMaterial.h"

// a sorter widget for ordering geometry.
struct GHRenderGeomLess
{
	__inline bool operator()(const GHRenderListEntry* node1, const GHRenderListEntry* node2) const 
	{ 
		if (!node1 && !node2) return false;
		if (!node1) return false;
		if (!node2) return true;
		
		// *** first by app render priority.
        if (node1->mDrawOrder != node2->mDrawOrder)
        {
            return node1->mDrawOrder < node2->mDrawOrder;
        }
        
		// *** then make sure the alpha geometry is drawn last and back to front.
		if (node1->mMaterial->isAlpha()) 
		{
			if (!node2->mMaterial->isAlpha()) return false;
			if (node1->mDistFromCam < node2->mDistFromCam) {
				return false;
			}
			if (node2->mDistFromCam < node1->mDistFromCam) {
				return true;
			}
		}
		else if (node2->mMaterial->isAlpha())
		{
			return true;
		}
		// *** then by render states to minimize changes.
		// if materials are the same sort by vertex buffer.
		if (node1->mMaterial == node2->mMaterial)
		{
			if (node1->mVertexBuffer != node2->mVertexBuffer) {
				return (node1->mVertexBuffer < node2->mVertexBuffer);
			}
		}
		
		// *** all other checks failed, do the ultimate fallback.
		return node1 < node2;
	}
};

GHRenderListContainer::GHRenderListContainer(unsigned int startSize)
: mStaticSize(startSize)
, mStaticEntries(new GHRenderListEntry[startSize])
{
	mOrderedGeometry.reserve(mStaticSize);
}

GHRenderListContainer::~GHRenderListContainer(void)
{
	delete [] mStaticEntries;
	std::vector<GHRenderListEntry*>::iterator listIter;
	for (listIter = mAdditionalEntries.begin(); listIter != mAdditionalEntries.end(); ++listIter)
	{
		delete *listIter;
	}
}

GHRenderListEntry* GHRenderListContainer::getEntryForAdd(void)
{
	GHRenderListEntry* ret = 0;
	if (mNumUsedEntries < mStaticSize) 
	{
		ret = &mStaticEntries[mNumUsedEntries];
	}
	else if (mNumUsedEntries - mStaticSize < mAdditionalEntries.size())
	{
		ret = mAdditionalEntries[mNumUsedEntries - mStaticSize];
	}
	else 
	{
		ret = new GHRenderListEntry;
		mAdditionalEntries.push_back(ret);
	}
	mOrderedGeometry.push_back(ret);
	mNumUsedEntries++;
	return ret;
}

void GHRenderListContainer::sort(void)
{
	GHRenderGeomLess sorter;
	std::sort(mOrderedGeometry.begin(), mOrderedGeometry.end(), sorter);
}

void GHRenderListContainer::reset(void)
{
	mOrderedGeometry.resize(0);
	mNumUsedEntries = 0;
}
