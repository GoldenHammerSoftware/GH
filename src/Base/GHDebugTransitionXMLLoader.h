// Copyright Golden Hammer Software
#include "GHXMLObjLoader.h"

// <debugtransition id="somethingrecognizable"/>
class GHDebugTransitionXMLLoader : public GHXMLObjLoader
{
public:
	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE
};
