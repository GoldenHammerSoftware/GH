// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHWPExitAppTransitionXMLLoader : public GHXMLObjLoader
{
public:
	GHWPExitAppTransitionXMLLoader(bool& exitFlag);

    virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
    GH_NO_POPULATE

private:
	bool& mExitFlag;
};
