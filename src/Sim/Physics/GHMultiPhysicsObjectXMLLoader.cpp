#include "GHMultiPhysicsObjectXMLLoader.h"
#include "GHMultiPhysicsObject.h"
#include "GHXMLObjFactory.h"
#include "GHXMLNode.h"

GHMultiPhysicsObjectXMLLoader::GHMultiPhysicsObjectXMLLoader(const GHXMLObjFactory& xmlObjFactory)
	: mXMLObjFactory(xmlObjFactory)
{

}

void* GHMultiPhysicsObjectXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	auto children = node.getChildren();
	size_t numChildren = children.size();
	GHMultiPhysicsObject* ret = new GHMultiPhysicsObject(numChildren);

	for (size_t i = 0; i < numChildren; ++i)
	{
		const GHXMLNode* child = children[i];
		GHPhysicsObject* subObject = (GHPhysicsObject*)mXMLObjFactory.load(*child, &extraData);
		if (subObject)
		{
			ret->addSubObject(subObject);
		}
	}

	return ret;
}
