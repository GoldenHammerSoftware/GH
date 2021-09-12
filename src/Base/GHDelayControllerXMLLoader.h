// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHTimeVal;

/*
<delayController time=30.0>
	(a loadable GHTransition goes here)
</delayController>
*/
class GHDelayControllerXMLLoader : public GHXMLObjLoader
{
public:
	GHDelayControllerXMLLoader(const GHTimeVal& time, GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHTimeVal& mTime;
	GHXMLObjFactory& mObjFactory;
};
