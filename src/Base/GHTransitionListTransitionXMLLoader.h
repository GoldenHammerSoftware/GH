// Copyright Golden Hammer Software
#include "GHXMLObjLoader.h"

//xml loader for a list of transitions that is also a transition
class GHTransitionListTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHTransitionListTransitionXMLLoader(const GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHXMLObjFactory& mObjFactory;
};
