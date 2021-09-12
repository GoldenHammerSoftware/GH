// Copyright Golden Hammer Software
#include "GHHeightFieldBlitterCache.h"
#include "GHVBFactory.h"

GHHeightFieldBlitterCache::GHHeightFieldBlitterCache(GHVBFactory& vbFactory)
: mVBFactory(vbFactory)
{
}

GHHeightFieldBlitterCache::~GHHeightFieldBlitterCache(void)
{
	std::map<GHPoint2i, GHVBBlitterPtr*>::iterator iter;
	for (iter = mBlitters.begin(); iter != mBlitters.end(); ++iter)
	{
		iter->second->release();
	}
}

GHVBBlitterPtr* GHHeightFieldBlitterCache::getBlitter(const GHPoint2i& meshSize, unsigned int numIndices)
{
	std::map<GHPoint2i, GHVBBlitterPtr*>::iterator findIter = mBlitters.find(meshSize);
	if (findIter != mBlitters.end()) {
		return findIter->second;
	}
	GHVBBlitter* blitter = mVBFactory.createIBStream(numIndices, GHVBUsage::STATIC); 
	GHVBBlitterPtr* blitterPtr = new GHVBBlitterPtr(blitter);
	blitterPtr->acquire();
	mBlitters[meshSize] = blitterPtr;
	return blitterPtr;
}

