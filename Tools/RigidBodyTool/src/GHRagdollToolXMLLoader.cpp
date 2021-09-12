// Copyright 2010 Golden Hammer Software
#include "GHRagdollToolXMLLoader.h"
#include "GHXMLNode.h"
#include "GHEntity.h"
#include "GHTransform.h"
#include "GHXMLObjFactory.h"
#include "GHStringHashTable.h"
#include "GHEntitySpawnTransition.h"
#include "GHControllerTransition.h"
#include "GHRagdollTool.h"
#include "GHEntityHashes.h"
#include "GHXMLSerializer.h"

GHRagdollToolXMLLoader::GHRagdollToolXMLLoader(const GHXMLObjFactory& objFactory,
                                               const GHIdentifierMap<int>& enumStore,
                                               const GHStringHashTable& hashtable,
                                               GHControllerMgr& controllerMgr,
                                               GHPhysicsSim& physicsSim,
                                               const GHXMLSerializer& xmlSerializer)
: mObjFactory(objFactory)
, mEnumStore(enumStore)
, mHashtable(hashtable)
, mControllerMgr(controllerMgr)
, mPhysicsSim(physicsSim)
, mXMLSerializer(xmlSerializer)
{
    
}

void* GHRagdollToolXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* entity = extraData.getProperty(GHEntityHashes::P_ENTITY);
    
    GHIdentifier activeID = mHashtable.generateHash(node.getAttribute("activeID"));
    
    GHRagdollGenerator* generator = new GHRagdollGenerator(mObjFactory,
                                                           mEnumStore,
                                                           entity,
                                                           mPhysicsSim,
                                                           mControllerMgr,
                                                           activeID);
    
    loadDescription(*generator, node);

    generator->generateRagdoll();
    return generator;
}

void GHRagdollToolXMLLoader::loadDescription(GHRagdollGenerator& ragdollGenerator, const GHXMLNode& node) const
{
    const std::vector<GHXMLNode*>& nodes = node.getChildren();
    size_t numChildren = nodes.size();
    ragdollGenerator.mDescription.mNodes.reserve(numChildren);
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* node = nodes[i];
        ragdollGenerator.mDescription.mNodes.push_back(GHRagdollGeneratorDescription::NodeDesc());
        GHRagdollGeneratorDescription::NodeDesc& nodeDesc = ragdollGenerator.mDescription.mNodes.back();
        
        nodeDesc.mParentName.setConstChars(node->getAttribute("target"), GHString::CHT_COPY);
        nodeDesc.mTargetName.setConstChars(nodeDesc.mParentName, GHString::CHT_COPY);
        nodeDesc.mPivotName.setConstChars(nodeDesc.mParentName, GHString::CHT_COPY);
        nodeDesc.mCenterName.setConstChars("", GHString::CHT_REFERENCE);
        
        bool hasPivot = false;
        const char* parentAttr = node->getAttribute("parent");
        if (parentAttr)
        {
            nodeDesc.mParentName.setConstChars(parentAttr, GHString::CHT_COPY);
            const char* pivotAttr = node->getAttribute("pivot");
            if (pivotAttr) {
                nodeDesc.mPivotName.setConstChars(pivotAttr, GHString::CHT_COPY);
                hasPivot = true;
            }
        }
        
        const char* centerAttr = node->getAttribute("center");
        if (centerAttr)
        {
            nodeDesc.mCenterName.setConstChars(centerAttr, GHString::CHT_COPY);
        }
        else if (!hasPivot)
        {
            nodeDesc.mCenterName = nodeDesc.mTargetName;
        }
        
        nodeDesc.mParentID = mHashtable.generateHash(nodeDesc.mParentName);
        nodeDesc.mTargetID = mHashtable.generateHash(nodeDesc.mTargetName);
        nodeDesc.mPivotID = mHashtable.generateHash(nodeDesc.mPivotName);
        if (strcmp(nodeDesc.mCenterName.getChars(), "")) {
            nodeDesc.mCenterID = mHashtable.generateHash(nodeDesc.mCenterName);
        }
        
        nodeDesc.mRadius = .1f;
        node->readAttrFloat("radius", nodeDesc.mRadius);
    }
}