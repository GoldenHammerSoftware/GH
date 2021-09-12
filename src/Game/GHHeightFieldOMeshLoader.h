// Copyright Golden Hammer Software
#pragma once

#include "GHMath/GHPoint.h"
#include <vector>
#include "GHHeightFieldMeshDesc.h"
#include "GHString/GHIdentifierMap.h"

class GHModel;
class GHHeightField;
class GHXMLNode;
class GHXMLObjFactory;
class GHStringIdFactory;
class GHVBFactory;
class GHHeightFieldOMeshBucket;
class GHThreadFactory;
class GHResourceFactory;
class GHTextureWorldMap;

// looks at a heightfield and creates an optimized set of meshes.
class GHHeightFieldOMeshLoader
{
public:
    GHHeightFieldOMeshLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
                             GHVBFactory& vbFactory, const GHThreadFactory& threadFactory,
							 const GHIdentifierMap<int>& enumMap,
							 GHResourceFactory& resFactory);
    
    void createRenderable(GHModel& ret, const GHHeightField& hf, const GHXMLNode& node) const;
    
private:
    // create minimum mesh buckets containing only the corner nodes.
    void createStarterBuckets(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                              GHModel& ret,
                              const GHHeightField& hf,
                              const GHXMLNode& node,
							  GHTextureWorldMap* weightMap) const;
    
    // remove unimportant nodes to the mesh until we're as complex as we care to be.
    void simplifyMesh(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                      const GHHeightField& hf) const;

    // create something that can draw all of our mesh buckets.
    void createMeshRenderable(std::vector<GHHeightFieldOMeshBucket*>& buckets,
                              GHModel& ret,
                              const GHHeightField& hf,
							  const float* uv) const;
    
private:
    GHXMLObjFactory& mObjFactory;
    const GHStringIdFactory& mIdFactory;
    GHVBFactory& mVBFactory;
    const GHThreadFactory& mThreadFactory;
	const GHIdentifierMap<int>& mEnumMap;
	GHResourceFactory& mResourceFactory;
};
