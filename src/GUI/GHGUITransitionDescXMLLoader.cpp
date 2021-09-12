// Copyright Golden Hammer Software
#include "GHGUITransitionDescXMLLoader.h"
#include "GHGUITransitionDesc.h"
#include "GHXMLNode.h"

GHGUITransitionDescXMLLoader::GHGUITransitionDescXMLLoader(const GHStringIdFactory& hashTable)
: mIdFactory(hashTable)
{
}
    
void* GHGUITransitionDescXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUITransitionDesc* ret = new GHGUITransitionDesc;
	populate(ret, node, extraData);
	return ret;
}
    
void GHGUITransitionDescXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUITransitionDesc* desc = (GHGUITransitionDesc*)obj;
    node.readAttrFloat("transitionTime", desc->mTransitionTime);
    node.readAttrFloat("transitionWait", desc->mTransitionWait);
    const char* transitionPropStr = node.getAttribute("transitionProp");
    if (transitionPropStr) {
        desc->mTransitionProp = mIdFactory.generateHash(transitionPropStr);
    }
}
    
