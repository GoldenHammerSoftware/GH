// Copyright Golden Hammer Software
#include "GHModel.h"
#include "GHModelAnimControllerCloner.h"
#include "GHGeometryRenderable.h"
#include "GHPhysicsObject.h"
#include "GHPhysicsSim.h"
#include "GHRenderGroup.h"
#include "GHGeometry.h"
#include "GHTransformAnimController.h"
#include "GHSkinAnimController.h"
#include "GHFrameAnimControllerVB.h"
#include "GHMaterialReplacer.h"
#include "GHSkeletonAnimController.h"

GHModel::GHModel(void)
: mRenderable(0)
, mSkeleton(0)
, mPhysicsObject(0)
, mCloneSource(0)
, mAnimRenderCallback(*this)
{
}

GHModel::~GHModel(void)
{
    // first unattach all children, otherwise we'll delete nodes they are attached to.
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->getSkeleton()->detach();
    }
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->release();
    }
    
    for (size_t i = 0; i < mAnimControllers.size(); ++i)
    {
        delete mAnimControllers[i];
    }
    for (size_t i = 0; i < mAnimCloners.size(); ++i)
    {
        delete mAnimCloners[i];
    }

	delete mRenderable;
	delete mPhysicsObject;
    if (mSkeleton)
    {
        mSkeleton->detach();
        delete mSkeleton;
    }
    for (size_t i = 0; i < mBounds.size(); ++i) {
        delete mBounds[i];
    }
    GHRefCounted::changePointer((GHRefCounted*&)mCloneSource, 0);
}

void GHModel::setRenderable(GHGeometryRenderable* renderable) 
{ 
    if (mRenderable) {
        delete mRenderable;
    }
    mRenderable = renderable; 
    if (mRenderable) {
        for (size_t i = 0; i < mBounds.size(); ++i) {
            mRenderable->addBounds(*mBounds[i]);
        }
        mRenderable->setRenderCallback(&mAnimRenderCallback);
    }
}

void GHModel::setEntityData(const GHPropertyContainer* entData)
{
    if (mRenderable) mRenderable->setEntityData(entData);
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->setEntityData(entData);
    }
}

void GHModel::setSkeleton(GHTransformNode* skeleton)
{
    mSkeleton = skeleton;
    if (mPhysicsObject) 
    {
        mPhysicsObject->setTransformNode(skeleton);
    }
}

void GHModel::setPhysicsObject(GHPhysicsObject* physicsObject)
{
    mPhysicsObject = physicsObject;
    if (mSkeleton && mPhysicsObject)
    {
        mPhysicsObject->setTransformNode(mSkeleton);
    }
}

void GHModel::addRenderableToScene(GHRenderGroup& renderer, const GHIdentifier& renderType)
{
    if (mRenderable) renderer.addRenderable(*mRenderable, renderType);
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->addRenderableToScene(renderer, renderType);
    }
}

void GHModel::removeRenderableFromScene(GHRenderGroup& renderer, const GHIdentifier& renderType)
{
    if (mRenderable) renderer.removeRenderable(*mRenderable, renderType);
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->removeRenderableFromScene(renderer, renderType);
    }
}

void GHModel::addPhysicsObjectToSim(GHPhysicsSim& physicsSim)
{
    if (mSkeleton && mPhysicsObject) physicsSim.addToSim(*mPhysicsObject);
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->addPhysicsObjectToSim(physicsSim);
    }
}

void GHModel::removePhysicsObjectFromSim(GHPhysicsSim& physicsSim)
{
    if (mSkeleton && mPhysicsObject) {
        physicsSim.removeFromSim(*mPhysicsObject);
    }
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->removePhysicsObjectFromSim(physicsSim);
    }
}

void GHModel::addTransformAnimController(GHTransformAnimController* controller, GHModelAnimControllerCloner* cloner)
{
    mTransformAnimControllers.push_back(controller);
    mAnimControllers.push_back(controller);
    mAnimCloners.push_back(cloner);
}

void GHModel::addGeometryAnimController(GHFrameAnimControllerVB* controller, GHModelAnimControllerCloner* cloner)
{
    mGeoAnimControllers.push_back(controller);
    mAnimControllers.push_back(controller);
    mAnimCloners.push_back(cloner);
}

void GHModel::addSkeletonAnimController(GHSkeletonAnimController* controller, GHModelAnimControllerCloner* cloner)
{
    mSkelAnimControllers.push_back(controller);
    mAnimControllers.push_back(controller);
    mAnimCloners.push_back(cloner);
}


void GHModel::addAnimController(GHAnimController* controller, GHModelAnimControllerCloner* cloner)
{
    mAnimControllers.push_back(controller);
    mAnimCloners.push_back(cloner);
}

void GHModel::removeAnimController(GHAnimController* controller)
{
	std::vector<GHAnimController*>::iterator conIter;
	std::vector<GHModelAnimControllerCloner*>::iterator cloneIter = mAnimCloners.begin();
	for (conIter = mAnimControllers.begin(); conIter != mAnimControllers.end(); ++conIter)
	{
		if (*conIter == controller)
		{
			mAnimControllers.erase(conIter);
			mAnimCloners.erase(cloneIter);
			return;
		}
		cloneIter++;
	}
}

void GHModel::addBounds(const GHSphereBounds& bounds)
{
    GHSphereBounds* newBounds = new GHSphereBounds(bounds);
    mBounds.push_back(newBounds);
    if (mRenderable) {
        mRenderable->addBounds(*newBounds);
    }
}

void GHModel::setAnim(const GHIdentifier& animId, float startTime)
{
    for (size_t i = 0; i < mAnimControllers.size(); ++i)
    {
        mAnimControllers[i]->setAnim(animId, startTime);
    }
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->setAnim(animId, startTime);
    }
}

void GHModel::setAnimBlend(const std::vector<GHAnimController::AnimBlend>& blends)
{
    for (size_t i = 0; i < mAnimControllers.size(); ++i)
    {
        mAnimControllers[i]->setAnimBlend(blends);
    }
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->setAnimBlend(blends);
    }
}

void GHModel::playRenderAnims(void)
{
    for (size_t i = 0; i < mAnimControllers.size(); ++i)
    {
        mAnimControllers[i]->update();
    }
}

GHModel* GHModel::clone(void)
{
    GHModel* ret = new GHModel();
    if (mCloneSource) ret->setCloneSource(mCloneSource);
    else ret->setCloneSource(this);
    
    // detach children before cloning.
    std::vector<GHTransformNode*> childParents;
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        childParents.push_back(mSubModels[i]->getSkeleton()->getParent());
        mSubModels[i]->getSkeleton()->getParent()->removeChild(mSubModels[i]->getSkeleton());
    }

    // clone skeleton, and store in a way that is easy to map to the original.
    std::vector<GHTransformNode*> myNodes;
    std::vector<GHTransformNode*> cloneNodes;
    GHTransformNode* cloneSkel = 0;
    if (mSkeleton)
    {
        cloneSkel = mSkeleton->clone(0);
        mSkeleton->collectTree(myNodes);
        cloneSkel->collectTree(cloneNodes);
        ret->setSkeleton(cloneSkel);
    }
    
    // clone geometry.
    if (mRenderable)
    {
        GHGeometryRenderable* newRenderable = new GHGeometryRenderable();
        ret->setRenderable(newRenderable);
        for (size_t geoIdx = 0; geoIdx < mRenderable->getGeometry().size(); ++geoIdx)
        {
            GHGeometry* oldGeo = mRenderable->getGeometry()[geoIdx];
            GHGeometry* newGeo = oldGeo->clone();
            newRenderable->addGeometry(newGeo);
            for (size_t nodeIdx = 0; nodeIdx < myNodes.size(); ++nodeIdx)
            {
                if (oldGeo->getTransform() == myNodes[nodeIdx])
                {
                    newGeo->setTransform(cloneNodes[nodeIdx]);
                    break;
                }
            }
        }
    }
    
    // clone bounds.
    GHSphereBounds newBounds;
    for (size_t i = 0; i < mBounds.size(); ++i)
    {
        for (size_t nodeIdx = 0; nodeIdx < myNodes.size(); ++nodeIdx)
        {
            if (mBounds[i]->getTransform() == myNodes[nodeIdx])
            {
                newBounds.setSphere(mBounds[i]->getLocalSphere());
                newBounds.setTransform(cloneNodes[nodeIdx]);
                ret->addBounds(newBounds);
                break;
            }
        }
    }
    
    // clone anim controllers.
    for (size_t i = 0; i < mAnimCloners.size(); ++i)
    {
        mAnimCloners[i]->clone(*this, myNodes, *ret, cloneNodes);
    }

    // clone children and re-attach them.
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        GHModel* child = mSubModels[i]->clone();
        ret->addSubModel(child);
        ret->getSkeleton()->findChild(childParents[i]->getId())->addChild(child->getSkeleton());
        // and re-attach our models.
        childParents[i]->addChild(mSubModels[i]->getSkeleton());
    }
    
    //Physics clone happens after we reattach submodels because
    // the physics object might need to look at the combined tree (eg for ragdolls). 
    if (mPhysicsObject) {
        ret->setPhysicsObject(mPhysicsObject->clone(ret->mSkeleton));
    }
    return ret;
}

void GHModel::setCloneSource(GHRefCounted* src)
{
    GHRefCounted::changePointer((GHRefCounted*&)mCloneSource, src);
}

void GHModel::addSubModel(GHModel* model)
{
    model->acquire();
    mSubModels.push_back(model);
}

void GHModel::applyMatReplacements(GHMaterialReplacer& replacer)
{
    if (mRenderable) replacer.applyReplacements(*mRenderable);
    for (size_t i = 0; i < mSubModels.size(); ++i)
    {
        mSubModels[i]->applyMatReplacements(replacer);
    }
}
