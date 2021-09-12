#include "GHTransitionMessageHandlerXMLLoader.h"
#include "GHUtils/GHTransitionListTransition.h"
#include "GHUtils/GHTransitionMessageHandler.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"

GHTransitionMessageHandlerXMLLoader::GHTransitionMessageHandlerXMLLoader(const GHXMLObjFactory& objFactory)
	: mObjFactory(objFactory)
{

}

void* GHTransitionMessageHandlerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHTransition* transition = nullptr;

	const auto& xmlChildren = node.getChildren();
	size_t numChildren = xmlChildren.size();
	if (!numChildren) {
		return nullptr;
	}

	if (numChildren == 1) {
		transition = (GHTransition*)mObjFactory.load(*xmlChildren[0], &extraData);
	}
	else
	{
		GHTransitionListTransition* transitionList = new GHTransitionListTransition(numChildren);
		for (size_t i = 0; i < numChildren; ++i)
		{
			transitionList->addTransition((GHTransition*)mObjFactory.load(*xmlChildren[i], &extraData));
		}
		transition = transitionList;
	}

	return new GHTransitionMessageHandler(transition);
}
