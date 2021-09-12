// Copyright Golden Hammer Software
#pragma once

#include <vector>

class GHVertexBuffer;
class GHMaterial;
class GHTransform;
class GHPropertyContainer;

struct GHRenderListEntry 
{
	GHVertexBuffer* mVertexBuffer;
	GHMaterial* mMaterial;
	const GHTransform* mTransform;
    const GHPropertyContainer* mGeoData;
    const GHPropertyContainer* mEntityData;
	float mDistFromCam;
    float mDrawOrder;
};

// A very efficient way to store, sort, and blit a bunch of things.
class GHRenderListContainer
{
public:
	GHRenderListContainer(unsigned int startSize);
	~GHRenderListContainer(void);
	
	GHRenderListEntry* getEntryForAdd(void);
	void sort(void);
	void reset(void);
	
	const std::vector<GHRenderListEntry*>& getEntries(void) const { return mOrderedGeometry; }
	
private:
	unsigned int mStaticSize;
	GHRenderListEntry* mStaticEntries;
	std::vector<GHRenderListEntry*> mAdditionalEntries;
	unsigned int mNumUsedEntries;
	
	std::vector<GHRenderListEntry*> mOrderedGeometry;
};
