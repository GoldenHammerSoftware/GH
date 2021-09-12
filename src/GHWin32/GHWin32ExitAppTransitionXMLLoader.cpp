// Copyright Golden Hammer Software
#include "GHWin32ExitAppTransitionXMLLoader.h"
#include "GHWin32ExitAppTransition.h"

void* GHWin32ExitAppTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	return new GHWin32ExitAppTransition;
}
