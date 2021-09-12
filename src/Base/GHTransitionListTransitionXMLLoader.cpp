// Copyright Golden Hammer Software
#include "GHTransitionListTransitionXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHUtils/GHTransitionListTransition.h"

GHTransitionListTransitionXMLLoader::GHTransitionListTransitionXMLLoader(const GHXMLObjFactory& objFactory)
	: mObjFactory(objFactory)
{

}

void* GHTransitionListTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const GHXMLNode::NodeList& nodeList = node.getChildren();
	size_t numTransitions = nodeList.size();

	GHTransitionListTransition* ret = new GHTransitionListTransition(numTransitions);

	for (size_t i = 0; i < numTransitions; ++i)
	{
		const GHXMLNode* childNode = nodeList[i];
		GHTransition* trans = (GHTransition*)mObjFactory.load(*childNode, &extraData);
		if (trans) { ret->addTransition(trans); }
	}

	return ret;
}
