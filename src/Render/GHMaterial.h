// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include <vector>
#include "GHRenderPassMembership.h"
#include "GHMaterialCallbackType.h"

class GHTransform;
class GHMaterialParamHandle;
class GHViewInfo;
class GHMaterialCallback;
class GHPropertyContainer;
class GHVertexBuffer;

// Defines rendering parameters for objects
class GHMaterial : public GHResource
{
public:
    virtual void beginMaterial(const GHViewInfo& viewInfo) = 0;
    virtual void beginVB(const GHVertexBuffer& vb) {} // todo: make pure virtual.
    virtual void beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo) = 0;
    virtual void beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo) = 0;
    virtual void beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo) = 0;
    virtual void endMaterial(void) = 0;

    virtual void preBlit(void) {} // todo: make pure virtual.

    // Get a handle for modifying a parameter if this material supports it.
    //  otherwise return null.
    virtual GHMaterialParamHandle* getParamHandle(const char* paramName) = 0;
    // Add a callback for a render stage.
    // allows modification of values from the out side on the fly.
    void addCallback(GHMaterialCallbackType::Enum type, GHMaterialCallback* cb);
    
    bool isAlpha(void) const { return mIsAlpha; }
    void setIsAlpha(bool val) { mIsAlpha = val; }
    float getDrawOrder(void) const { return mDrawOrder; }
    void setDrawOrder(float val) { mDrawOrder = val; }
    
    GHRenderPassMembership& getPassMembership(void) { return mPassMembership; }
    GHMaterial* getPassOverride(const GHIdentifier& id);
    void addPassOverride(const GHIdentifier& id, GHMaterial* mat);
    
	bool hasLOD(void) const { return mLODs.size() > 0; }
	void addLOD(GHMaterial* mat, float minDist);
	GHMaterial* getLOD(float dist);

protected:
    virtual ~GHMaterial(void);
    void applyCallbacks(GHMaterialCallbackType::Enum type, const void* data);
    
private:
    typedef std::vector<GHMaterialCallback*> CallbackList;
    CallbackList mCallbacks[GHMaterialCallbackType::CT_MAX];
    
protected:
    // used for sorting.  we don't want to do a ton of virtual checks during sort.
    // subclasses are responsible for setting this value.
    bool mIsAlpha;
    float mDrawOrder;
    // used to make objects that use this material ignore certain render passes.
    GHRenderPassMembership mPassMembership;
    // set of materials that we use instead of this one for various render passes.
    std::map<GHIdentifier, GHMaterial*> mPassOverrides;

	// lod materials, sorted by closest to farthest min dist.
	// if empty we draw the main material
	// otherwise we draw the farthest mat that is outside the min dist.
	struct MaterialLOD
	{
		GHMaterial* mMat;
		// we do not draw this LOD if not within the min dist.
		float mMinDist;
	};
	std::vector<MaterialLOD> mLODs;
};
