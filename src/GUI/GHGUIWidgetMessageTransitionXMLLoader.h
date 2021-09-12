// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHMessageHandler;

// loads a GHMessageTransition that gets the parent widget as a prop val
// <guiWidgetMessage messageId=M_SELECTEDBUTTON propId=P_WIDGET/>
class GHGUIWidgetMessageTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHGUIWidgetMessageTransitionXMLLoader(GHStringIdFactory& hashTable, GHMessageHandler& messageHandler);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHStringIdFactory& mIdFactory;
	GHMessageHandler& mMessageHandler;
};
