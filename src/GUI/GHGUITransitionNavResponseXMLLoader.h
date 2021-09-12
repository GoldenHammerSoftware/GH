// Copyright Golden Hammer Software
#include "GHXMLObjLoader.h"

class GHXMLObjFactory;

class GHGUITransitionNavResponseXMLLoader : public GHXMLObjLoader
{
public:
	GHGUITransitionNavResponseXMLLoader(const GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHXMLObjFactory& mObjFactory;
};
