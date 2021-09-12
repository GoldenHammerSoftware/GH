// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

/*
<guiSizeTransition sizeChange="1.2"/>          
*/
class GHGUISizeTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHGUISizeTransitionXMLLoader(void);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE
};
