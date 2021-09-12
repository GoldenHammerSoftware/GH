// Copyright Golden Hammer Software
#include "GHGUITransitionNavResponseXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"
#include "GHGUITransitionNavResponse.h"

GHGUITransitionNavResponseXMLLoader::GHGUITransitionNavResponseXMLLoader(const GHXMLObjFactory& objFactory)
	: mObjFactory(objFactory)
{

}

void* GHGUITransitionNavResponseXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (!node.getChildren().size() || !node.getChildren()[0]) { return 0; }

	const GHXMLNode* transitionNode = node.getChildren()[0];
	GHTransition* trans = (GHTransition*)mObjFactory.load(*transitionNode, &extraData);

	if (!trans) { return 0; }

	return new GHGUITransitionNavResponse(trans);
}
