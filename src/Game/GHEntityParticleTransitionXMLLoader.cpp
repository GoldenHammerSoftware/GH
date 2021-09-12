// Copyright Golden Hammer Software
#include "GHEntityParticleTransitionXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHParticleSystemDesc.h"
#include "GHEntityHashes.h"
#include "GHEntity.h"
#include "GHModel.h"
#include "GHParticleAnimController.h"
#include "GHParticleTransition.h"
#include "GHModelParticleAnimCloner.h"
#include "GHGeometryRenderable.h"
#include "GHString/GHStringIdFactory.h"

GHEntityParticleTransitionXMLLoader::GHEntityParticleTransitionXMLLoader(GHXMLObjFactory& xmlFactory, 
	const GHTimeVal& time, const GHViewInfo& viewInfo, GHVBFactory& vbFactory, GHStringIdFactory& hashTable)
: mXMLFactory(xmlFactory)
, mTime(time)
, mViewInfo(viewInfo)
, mVBFactory(vbFactory)
, mIdFactory(hashTable)
{
}

void* GHEntityParticleTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
	if (!parentEnt) {
		GHDebugMessage::outputString("GHEntityParticleTransitionXMLLoader loaded with no parent entity.");
		return 0;
	}
	GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
	if (!parentModel) {
		GHDebugMessage::outputString("GHEntityParticleTransitionXMLLoader called with no model.");
		return 0;
	}
	GHGeometryRenderable* parentRenderable = parentModel->getRenderable();
	if (!parentRenderable) {
		GHDebugMessage::outputString("GHEntityParticleTransitionXMLLoader parent model has no renderable");
		return 0;
	}

	const GHXMLNode* descParent = node.getChild("particle", false);
	if (!descParent) {
		GHDebugMessage::outputString("No particle parent specified for particle transition");
		return 0;
	}
	if (!descParent->getChildren().size()) {
		GHDebugMessage::outputString("No particle specified for particle transition");
		return 0;
	}
	GHParticleSystemDescResource* desc = (GHParticleSystemDescResource*)mXMLFactory.load(*descParent->getChildren()[0], &extraData);
	if (!desc)
	{
		GHDebugMessage::outputString("Failed to load particle desc for entity particle transition");
		return 0;
	}

	GHParticleAnimController* anim = new GHParticleAnimController(desc, mTime, mViewInfo, mVBFactory);

	GHTransformNode* targetNode = parentModel->getSkeleton();
	assert(targetNode);
	const char* targetNodeName = descParent->getAttribute("targetNode");
	if (targetNodeName)
	{
		GHIdentifier targetNodeId = mIdFactory.generateHash(targetNodeName);
		targetNode = targetNode->findChild(targetNodeId);
		if (!targetNode) {
			GHDebugMessage::outputString("Failed to find target node for particle system");
			targetNode = parentModel->getSkeleton();
		}
	}
	anim->setTarget(targetNode);
	
	parentModel->addAnimController(anim, new GHModelParticleAnimCloner(*anim));
	anim->attachRenderable(*parentRenderable);

	GHParticleTransition* ret = new GHParticleTransition(anim->getParticle());
	return ret;
}

