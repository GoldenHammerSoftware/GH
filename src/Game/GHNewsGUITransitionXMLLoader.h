// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHNewsMgr;

class GHNewsGUITransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHNewsGUITransitionXMLLoader(GHNewsMgr& news);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHNewsMgr& mNews;
};
