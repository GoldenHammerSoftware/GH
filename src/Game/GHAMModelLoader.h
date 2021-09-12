// Copyright Golden Hammer Software
#pragma once

#include "GHModelLoader.h"
#include "GHString/GHIdentifierMap.h"
#include "GHAMStructs.h"
#include <vector>

class GHFileOpener;
class GHVBFactory;
class GHGeometryRenderable;
class GHVBDescription;
class GHVertexBuffer;
class GHTransformNode;
class GHAMAnimFactory;
class GHGeometry;
class GHSphereBounds;

// <gham file=blah.gham combinedBounds=false/>
class GHAMModelLoader : public GHModelLoader
{
public:
    GHAMModelLoader(const GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
                    const GHFileOpener& fileOpener, GHVBFactory& vbFactory,
                    const GHIdentifierMap<int>& enumStore, GHAMAnimFactory* animFactory);
    ~GHAMModelLoader(void);
    
    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE
    
private:
    void loadGHAM(GHModel& model, bool combinedBounds, const char* buffer, GHTransformNode* modelTopNode) const;
    const char* loadSkeleton(GHModel& model, int numNodes, const char* buffer, GHTransformNode* modelTopNode) const;
    const char* loadMesh(GHModel& model, bool combinedBounds, const char* buffer, GHTransformNode* modelTopNode, const GHAM::Header& ghamHeader) const;
    const char* loadMeshSkinDesc(std::vector<GHIdentifier>& skinBones, const char* buffer) const;
    const char* loadMeshAnim(const GHAM::MeshHeader& meshHeader, GHGeometry& geo, GHModel& model, const char* buffer) const;
    const char* loadStreamDescriptions(const GHAM::MeshHeader& meshHeader, GHVBDescription& vbDesc, int numStreams, const char* buffer) const;
    const char* loadStreamData(GHVertexBuffer& vb, int numStreams, const char* buffer) const;
    const char* loadIBData(GHVertexBuffer& vb, const char* buffer) const;
    const char* loadMeshBounds(GHSphereBounds*& ret, const char* buffer) const;
	
	// Check if we need to add an additional stream to a model.
	// If so, add a new stream description to vbDesc and return true.
	bool addGeneratedStreamDescription(GHVBDescription& vbDesc) const;
	void populateGeneratedVertexComponents(GHVertexBuffer& vb) const;

private:
    const GHFileOpener& mFileOpener;
    GHVBFactory& mVBFactory;
    const GHIdentifierMap<int>& mEnumStore;
    GHAMAnimFactory* mAnimFactory;
};
