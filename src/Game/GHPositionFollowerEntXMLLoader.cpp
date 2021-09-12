// Copyright Golden Hammer Software
#include "GHPositionFollowerEntXMLLoader.h"
#include "GHTransformNodeFollower.h"
#include "GHPositionFollower.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHEntityHashes.h"
#include "GHEntity.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHModel.h"
#include "GHTransformNode.h"
#include "GHUtils/GHControllerTransition.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"

GHPositionFollowerEntXMLLoader::GHPositionFollowerEntXMLLoader(GHTransform& other, GHControllerMgr& conMgr,
                                                               bool copyToEnt, const GHIdentifierMap<int>& enumMap,
															   GHStringIdFactory& hashTable)
: mOther(other)
, mConMgr(conMgr)
, mCopyToEnt(copyToEnt)
, mEnumMap(enumMap)
, mIdFactory(hashTable)
{
}

void* GHPositionFollowerEntXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHEntity* parentEnt = extraData.getProperty(GHEntityHashes::P_ENTITY);
    if (!parentEnt) {
        GHDebugMessage::outputString("GHPositionFollowerEntXMLLoader loaded with no parent entity.");
        return 0;
    }
    GHModel* parentModel = parentEnt->mProperties.getProperty(GHEntityHashes::MODEL);
    if (!parentModel) {
        GHDebugMessage::outputString("GHPositionFollowerEntXMLLoader called with no model.");
        return 0;
    }
    if (!parentModel->getSkeleton()) {
        GHDebugMessage::outputString("GHPositionFollowerEntXMLLoader ent has no skeleton.");
        return 0;
    }
    float priority = 0;
    node.readAttrFloat("priority", priority);
    int priorityEnum = 0;
    if (node.readAttrEnum("priorityEnum", priorityEnum, mEnumMap)) {
        priority = (float)priorityEnum;
    }
    
	GHTransformNode* targetTrans = parentModel->getSkeleton();
	const char* targetChar = node.getAttribute("targetNode");
	if (targetChar)
	{
		GHIdentifier targId = mIdFactory.generateHash(targetChar);
		GHTransformNode* targNode = parentModel->getSkeleton()->findChild(targId);
		if (targNode) {
			targetTrans = targNode;
		}
		else {
			GHDebugMessage::outputString("PosFollower targetNode specified but not found.");
		}
	}

	GHController* ret = 0;
    if (mCopyToEnt) {
		ret = new GHPositionFollower(targetTrans->getLocalTransform(), mOther);
    }
    else {
		ret = new GHTransformNodeFollower(mOther, *targetTrans);
    }
    return new GHControllerTransition(mConMgr, ret, priority);
}
