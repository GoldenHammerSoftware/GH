#pragma once
#include "GHXMLObjLoader.h"

//a collection of physics objects that can be handled as a single one
class GHMultiPhysicsObjectXMLLoader : public GHXMLObjLoader
{
public:
	GHMultiPhysicsObjectXMLLoader(const GHXMLObjFactory& xmlObjFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHXMLObjFactory& mXMLObjFactory;
};

