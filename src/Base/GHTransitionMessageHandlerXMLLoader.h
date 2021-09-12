#pragma once

#include "GHXMLObjLoader.h"

// Loads a GHTransitionMessageHandler -- a message handler that triggers one or more transitions via message
// Does not support loading a message to register for, but that could be added as an optional parameter.
//  (it's expected to be embedded in a GHMessageHandlerTransition or something else that handles registration)
//<transitionMessageHandler>
//	<transition1/>
//  <transition2/>
//</transitionMessageHandler>
class GHTransitionMessageHandlerXMLLoader : public GHXMLObjLoader
{
public:
	GHTransitionMessageHandlerXMLLoader(const GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHXMLObjFactory& mObjFactory;
};
