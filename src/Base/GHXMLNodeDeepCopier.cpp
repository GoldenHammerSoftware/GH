// Copyright Golden Hammer Software
#include "GHXMLNodeDeepCopier.h"
#include "GHXMLNode.h"

GHXMLNode* GHXMLNodeDeepCopier::copy(const GHXMLNode& original)
{
    GHXMLNode* ret = new GHXMLNode;
    ret->setName(original.getName(), GHString::CHT_COPY);
    const GHXMLNode::AttributeMap& attributes = original.getAttributes();
    GHXMLNode::AttributeMap::const_iterator iter = attributes.begin(), iterEnd = attributes.end();
    for (; iter != iterEnd; ++iter)
    {
        ret->setAttribute(iter->first, GHString::CHT_COPY,
                          iter->second, GHString::CHT_COPY);
    }
    ret->setValue(original.getValue(), GHString::CHT_COPY);
    
    const GHXMLNode::NodeList& children = original.getChildren();
    size_t numChildren = children.size();
    for (size_t i = 0; i < numChildren; ++i)
    {
        const GHXMLNode* child = children[i];
        GHXMLNode* childCopy = copy(*child);
        ret->addChild(childCopy);
    }
    return ret;
}
