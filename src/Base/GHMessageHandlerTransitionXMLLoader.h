// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHEventMgr;

/*
<messageHandler message="id">
	// handler xml here.
</messageHandlerTransition>
*/
class GHMessageHandlerTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHMessageHandlerTransitionXMLLoader(GHEventMgr& eventMgr, const GHStringIdFactory& idFactory, const GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHEventMgr& mEventMgr;
	const GHStringIdFactory& mIdFactory;
	const GHXMLObjFactory& mObjFactory;
};

