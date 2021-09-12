// Copyright Golden Hammer Software
#pragma once

#include "GHVBBlitter.h"
#include "GHMath/GHPoint.h"
#include <map>

class GHVBFactory;

// storage and creation of vb blitters that are shared between height field meshes.
class GHHeightFieldBlitterCache
{
public:
	GHHeightFieldBlitterCache(GHVBFactory& vbFactory);
	~GHHeightFieldBlitterCache(void);

	GHVBBlitterPtr* getBlitter(const GHPoint2i& meshSize, unsigned int numIndices);

private:
	GHVBFactory& mVBFactory;
	std::map<GHPoint2i, GHVBBlitterPtr*> mBlitters;
};
