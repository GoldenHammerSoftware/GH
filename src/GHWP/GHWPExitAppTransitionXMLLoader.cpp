// Copyright 2010 Golden Hammer Software
#include "GHWPExitAppTransitionXMLLoader.h"
#include "GHValueSetterTransition.h"

GHWPExitAppTransitionXMLLoader::GHWPExitAppTransitionXMLLoader(bool& exitFlag)
: mExitFlag(exitFlag)
{
}


void* GHWPExitAppTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	return new GHValueSetterTransition<bool>(mExitFlag, true, false);
}

