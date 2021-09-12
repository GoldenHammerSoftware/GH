// Copyright Golden Hammer Software
#include "GHTransformNodeXMLLoader.h"
#include "GHString/GHStringIdFactory.h"
#include "GHXMLNode.h"
#include "GHTransformNode.h"

GHTransformNodeXMLLoader::GHTransformNodeXMLLoader(GHStringIdFactory& hashTable)
: mIdFactory(hashTable)
{
}

void* GHTransformNodeXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHTransformNode* ret = new GHTransformNode;
    
    if (node.readAttrFloatArr("transform", ret->getLocalTransform().getMatrix().getArr(), 16)) {
        ret->getLocalTransform().incrementVersion();
    }
    else
    {
        GHPoint3 rotate(0,0,0);
        GHPoint3 translate(0,0,0);
        float scale = 1.0f;
        node.readAttrFloatArr("rotation", rotate.getArr(), 3);
        node.readAttrFloatArr("translate", translate.getArr(), 3);
        node.readAttrFloat("scale", scale);
        
        ret->getLocalTransform().becomeYawPitchRollRotation(rotate[0], rotate[1], rotate[2]);
        ret->getLocalTransform().uniformScale(scale);
        ret->getLocalTransform().setTranslate(translate);
    }
    
    populate(ret, node, extraData);
    
    return ret;
}

void GHTransformNodeXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHTransformNode* ret = (GHTransformNode*)obj;

    const char* idStr = node.getAttribute("id");
    if (idStr)
    {
        ret->setId(mIdFactory.generateHash(idStr));
    }

    const GHXMLNode::NodeList& children = node.getChildren();
    for (size_t i = 0; i < children.size(); ++i)
    {
        // todo? change create() to load() for inherit
        GHTransformNode* child = (GHTransformNode*)create(*children[i], extraData);
        ret->addChild(child);
    }
}
