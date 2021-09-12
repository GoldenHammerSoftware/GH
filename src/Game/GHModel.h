// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResource.h"
#include "Sim/GHTransformNode.h"
#include <vector>
#include "Sim/GHSphereBounds.h"
#include "Game/GHModelAnimUpdateTrigger.h"
#include "Sim/GHAnimController.h"

class GHRenderable;
class GHGeometryRenderable;
class GHAnimController;
class GHRefCounted;
class GHRenderGroup;
class GHTimeVal;
class GHMaterialReplacer;
class GHGeometry;
class GHTransformAnimController;
class GHFrameAnimControllerVB;
class GHSkinAnimController;
class GHPhysicsSim;
class GHPhysicsObject;
class GHModelAnimControllerCloner;
class GHSkeletonAnimController;
class GHPropertyContainer;

// Wrapper for something that we'd load from a model file
// Contains rendering, physics, and anim info
// Useful for cloning new instances.
class GHModel : public GHResource
{
public:
    // todo: get rid of these.
    typedef std::vector<GHTransformAnimController*> TransformAnimList;
    typedef std::vector<GHFrameAnimControllerVB*> GeoAnimList;
    typedef std::vector<GHSkeletonAnimController*> SkelAnimList;
    
public:
    GHModel(void);

    // add/remove our renderables and children to a scene renderer.
    void addRenderableToScene(GHRenderGroup& renderer, const GHIdentifier& renderType);
    void removeRenderableFromScene(GHRenderGroup& renderer, const GHIdentifier& renderType);
    
    void addPhysicsObjectToSim(GHPhysicsSim& physicsSim);
    void removePhysicsObjectFromSim(GHPhysicsSim& physicsSim);
    
    void setRenderable(GHGeometryRenderable* renderable);
    GHGeometryRenderable* getRenderable(void) { return mRenderable; }
    const GHGeometryRenderable* getRenderable(void) const { return mRenderable; }
    void setEntityData(const GHPropertyContainer* entData);
    
    GHTransformNode* getSkeleton(void) { return mSkeleton; }
    const GHTransformNode* getSkeleton(void) const { return mSkeleton; }
    void setSkeleton(GHTransformNode* skeleton);    

    // addAnimController can be called when we don't need to later ask for the controller by type.
    // if a special subcategory add[subcat]controller function exists, that should be used instead.
    void addAnimController(GHAnimController* controller, GHModelAnimControllerCloner* cloner);
	void removeAnimController(GHAnimController* controller);
    void addTransformAnimController(GHTransformAnimController* controller, GHModelAnimControllerCloner* cloner);
    void addGeometryAnimController(GHFrameAnimControllerVB* controller, GHModelAnimControllerCloner* cloner);
    void addSkeletonAnimController(GHSkeletonAnimController* controller, GHModelAnimControllerCloner* cloner);
    
    const std::vector<GHAnimController*>& getAnimControllers(void) const { return mAnimControllers; }
    const TransformAnimList& getTransformAnimControllers(void) const { return mTransformAnimControllers; }
    const GeoAnimList& getGeoAnimControllers(void) const { return mGeoAnimControllers; }
    const SkelAnimList& getSkelAnimControllers(void) const { return mSkelAnimControllers; }
    
    void setAnim(const GHIdentifier& animId, float startTime=0);
    void setAnimBlend(const std::vector<GHAnimController::AnimBlend>& blends);
    // play the anims that should only happen when drawn
    void playRenderAnims(void);
    
    GHModel* clone(void);
    void setCloneSource(GHRefCounted* src);

    void addSubModel(GHModel* model);
    void addBounds(const GHSphereBounds& bounds);
    void applyMatReplacements(GHMaterialReplacer& replacer);
    
    void setPhysicsObject(GHPhysicsObject* physicsObject);
    GHPhysicsObject* getPhysicsObject(void) { return mPhysicsObject; }
    const GHPhysicsObject* getPhysicsObject(void) const { return mPhysicsObject; }
    std::vector<GHModel*>& getSubmodels(void) { return mSubModels; }
    const std::vector<GHModel*>& getSubmodels(void) const { return mSubModels; }
    
protected:
    ~GHModel(void);

private:
    GHTransformNode* mSkeleton;
    GHGeometryRenderable* mRenderable;
    std::vector<GHSphereBounds*> mBounds;
    GHPhysicsObject* mPhysicsObject;

    GHModelAnimUpdateTrigger mAnimRenderCallback;
    std::vector<GHAnimController*> mAnimControllers;
    std::vector<GHModelAnimControllerCloner*> mAnimCloners;
    TransformAnimList mTransformAnimControllers;
    GeoAnimList mGeoAnimControllers;
    SkelAnimList mSkelAnimControllers;
    
    std::vector<GHModel*> mSubModels;
    // if we were cloned, pointer to the thing we were cloned from.
    GHRefCounted* mCloneSource;
};
