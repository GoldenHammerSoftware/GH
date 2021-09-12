// Copyright Golden Hammer Software
#include "GHModelLoader.h"
#include "GHXMLNode.h"
#include "GHModel.h"
#include "GHXMLObjFactory.h"
#include "GHString/GHStringIdFactory.h"
#include "GHMaterialReplacer.h"
#include "GHFrameAnimSet.h"
#include "GHFrameAnimController.h"
#include "GHEntityHashes.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGeometryRenderable.h"

GHModelLoader::GHModelLoader(const GHXMLObjFactory& objFactory,
                             const GHStringIdFactory& hashTable)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
{
}

void* GHModelLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* ret = new GHModel;
    populate(ret, node, extraData);
    return ret;
}

void GHModelLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHModel* ret = (GHModel*)obj;
    GHPropertyStacker modelStacker(extraData, GHEntityHashes::MODEL, ret);
    
    loadTransformNode(*ret, node, true);
    loadChildren(*ret, node);
    loadMatReplace(*ret, node);
    loadAnimSet(*ret, node);
    loadPhysics(*ret, node, extraData);

	if (!ret->getRenderable())
	{
		GHGeometryRenderable* renderable = new GHGeometryRenderable();
		ret->setRenderable(renderable);
	}
}

void GHModelLoader::loadTransformNode(GHModel& ret, const GHXMLNode& node, bool createIfEmpty) const
{
    const GHXMLNode* transformXML = node.getChild("transformNode", false);
    GHTransformNode* transform = 0;
    if (transformXML) {
        transform = (GHTransformNode*)mObjFactory.load(*transformXML);
    }
    else {
        if (createIfEmpty) {
            transform = new GHTransformNode;
            transform->setId(mIdFactory.generateHash("ModelTop"));
        }
    }
    ret.setSkeleton(transform);
}

void GHModelLoader::loadChildren(GHModel& ret, const GHXMLNode& node) const
{
    const GHXMLNode* childrenNode = node.getChild("children", false);
    if (childrenNode)
    {
        const GHXMLNode::NodeList& children = childrenNode->getChildren();
        for (size_t i = 0; i < children.size(); ++i)
        {
            const GHXMLNode* childNode = children[i];
            if (childNode->getChildren().size())
            {
                const char* parentNodeName = childNode->getAttribute("parentNode");

                const GHXMLNode* submodelNode = childNode->getChildren()[0];
                GHModel* submodel = (GHModel*)mObjFactory.load(*submodelNode);
                if (submodel)
                {
                    ret.addSubModel(submodel);
                    GHTransformNode* parentNode = ret.getSkeleton();
                    if (parentNodeName)
                    {
                        GHIdentifier parentId = mIdFactory.generateHash(parentNodeName);
                        GHTransformNode* testNode = ret.getSkeleton()->findChild(parentId);
                        if (testNode) parentNode = testNode;
                        parentNode->addChild(submodel->getSkeleton());
                    }
                }
            }
        }
    }
}

void GHModelLoader::loadMatReplace(GHModel& ret, const GHXMLNode& node) const
{
    const GHXMLNode* matrepNode = node.getChild("matreplace", false);
    if (!matrepNode) return;
    GHMaterialReplacer* replacer = (GHMaterialReplacer*)mObjFactory.load(*matrepNode);
    if (!replacer) return;
    
    ret.applyMatReplacements(*replacer);
    
    delete replacer;
}

void GHModelLoader::loadAnimSet(GHModel& ret, const GHXMLNode& node) const
{
    const GHXMLNode* animSetNode = node.getChild("animSet", false);
    if (!animSetNode) return;
    GHFrameAnimSet* animSet = (GHFrameAnimSet*)mObjFactory.load(*animSetNode);
    if (!animSet) return;
    
    animSet->acquire();
    const std::vector<GHAnimController*>& controllers = ret.getAnimControllers();
    for (size_t i = 0; i < controllers.size(); ++i)
    {
        if (controllers[i]->getAnimType() == GHAnimController::AT_FRAME)
        {
            ((GHFrameAnimController*)controllers[i])->setAnimSet(animSet);
        }
    }
    animSet->release();
}

void GHModelLoader::loadPhysics(GHModel& ret, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const GHXMLNode* physicsNode = node.getChild("physics", false);
	if (!physicsNode) physicsNode = node.getChild("physicsList", false);
    if (!physicsNode) physicsNode = node.getChild("ragdoll", false);
    if (!physicsNode) physicsNode = node.getChild("vehicle", false);
    if (!physicsNode) return;
    GHPhysicsObject* physicsObject = (GHPhysicsObject*)mObjFactory.load(*physicsNode, &extraData);
    if (!physicsObject)
    {
    	GHDebugMessage::outputString("Failed to load physics object");
    	return;
    }
    ret.setPhysicsObject(physicsObject);
}

