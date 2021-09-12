// Copyright Golden Hammer Software
#include "GHHeightFieldEntXMLLoader.h"
#include "GHEntityXMLLoader.h"
#include "GHHeightFieldXMLLoader.h"
#include "GHEntity.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHHeightFieldProperties.h"
#include "GHHeightField.h"
#include "GHPlatform/GHRefCounted.h"
#include "GHUtils/GHProperty.h"
#include "GHHeightFieldNMeshLoader.h"
#include "GHModel.h"
#include "GHEntityHashes.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHProfiler.h"
#include "GHUtils/GHTransition.h"
#include "GHUtils/GHPropertyStacker.h"

GHHeightFieldEntXMLLoader::GHHeightFieldEntXMLLoader(GHXMLObjFactory& objFactory, const GHHeightFieldXMLLoader& heightFieldLoader, 
	const GHEntityXMLLoader& entLoader, GHVBFactory& vbFactory, GHStringIdFactory& hashTable, const GHThreadFactory& threadFactory,
	const GHIdentifierMap<int>& enumMap, GHResourceFactory& resourceFactory)
: mHeightFieldLoader(heightFieldLoader)
, mEntLoader(entLoader)
, mObjFactory(objFactory)
, mIdFactory(hashTable)
, mNMeshLoader(mObjFactory, mIdFactory, vbFactory, enumMap)
, mOMeshLoader(mObjFactory, mIdFactory, vbFactory, threadFactory, enumMap, resourceFactory)
{
}

void* GHHeightFieldEntXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* ret = new GHEntity;

    const GHXMLNode* hfNode = node.getChild("heightField", false);
    if (!hfNode) {
        GHDebugMessage::outputString("Failed to find heightField node for hfEnt");
        return ret;
    }
    
    GHHeightField* hf = (GHHeightField*)mHeightFieldLoader.create(*hfNode, extraData);
    if (!hf) {
        GHDebugMessage::outputString("Failed to create heightField for hfEnt");
        return ret;
    }
	GHPropertyStacker hfStack(extraData, GHHeightFieldProperties::HEIGHTFIELD, hf);

    ret->mProperties.setProperty(GHHeightFieldProperties::HEIGHTFIELD, GHProperty(hf, new GHRefCountedType<GHHeightField>(hf)));
    
    // assume textures are square and tiled, find the smallest of the two
	// override this from the model properties if it doesn't fit.
    unsigned int hfWidth,hfHeight;
    hf->getDimensions(hfWidth, hfHeight);
    unsigned int smallestDim = (hfWidth > hfHeight) ? hfHeight : hfWidth;
    float texWorldSize = (smallestDim)*hf->getDistBetweenNodes();
    ret->mProperties.setProperty(GHHeightFieldProperties::HFTEXTUREWORLDSIZE, texWorldSize);
    
    loadModel(*ret, node, extraData);
    
    mEntLoader.populate(ret, node, extraData);
    
    return ret;
}

// todo: move this to heightfield model loader
void GHHeightFieldEntXMLLoader::loadModel(GHEntity& ent, const GHXMLNode& node, 
	GHPropertyContainer& extraData) const
{
    GHHeightField* hf = (GHHeightField*)ent.mProperties.getProperty(GHHeightFieldProperties::HEIGHTFIELD);
    if (!hf) {
        GHDebugMessage::outputString("Failed to find heightfield on entity");
        return;
    }
	const GHXMLNode* modifiersNode = node.getChild("heightFieldModifiers", false);
	if (modifiersNode) {
		for (size_t i = 0; i < modifiersNode->getChildren().size(); ++i)
		{
			GHTransition* trans = (GHTransition*)mObjFactory.load(*modifiersNode->getChildren()[i], &extraData);
			if (trans) {
				trans->activate();
				trans->deactivate();
				delete trans;
			}
		}
	}

    GHModel* model = new GHModel();
    GHTransformNode* skeleton = new GHTransformNode;
    skeleton->setId(mIdFactory.generateHash("ModelTop"));
    model->setSkeleton(skeleton);
    ent.mProperties.setProperty(GHEntityHashes::MODEL, GHProperty(model, model));

    const GHXMLNode* oRendNode = node.getChild("optimizedHeightFieldRenderable", false);
    if (oRendNode) {
        mOMeshLoader.createRenderable(*model, *hf, *oRendNode);
    }
    else
    {
        const GHXMLNode* rendNode = node.getChild("heightFieldRenderable", false);
        if (!rendNode) {
            GHDebugMessage::outputString("Failed to find renderable node for hfModel");
            return;
        }
        mNMeshLoader.createRenderable(*model, *hf, *rendNode);
    }
    
    const GHXMLNode* phyNode = node.getChild("physics", false);
    if (phyNode)
    {
        GHProperty prevProperty = extraData.getProperty(GHHeightFieldProperties::HEIGHTFIELD);
        GHProperty prevModel = extraData.getProperty(GHEntityHashes::MODEL);
        extraData.setProperty(GHHeightFieldProperties::HEIGHTFIELD, hf);
        extraData.setProperty(GHEntityHashes::MODEL, model);
        GHPROFILEBEGIN("GHHeightFieldEntXMLLoader - physics")
        GHPhysicsObject* physicsObject = (GHPhysicsObject*)mObjFactory.load(*phyNode, &extraData);
        GHPROFILEEND("GHHeightFieldEntXMLLoader - physics")
        extraData.setProperty(GHHeightFieldProperties::HEIGHTFIELD, prevProperty);
        extraData.setProperty(GHEntityHashes::MODEL, prevModel);
        assert(physicsObject);
        model->setPhysicsObject(physicsObject);
    }
    else
    {
        GHDebugMessage::outputString("Could not load physics data for heightfield.");
    }
}

