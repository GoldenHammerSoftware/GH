// Copyright Golden Hammer Software
#include "GHDebugTransitionXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHDebugTransition.h"

void* GHDebugTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	return new GHDebugTransition(node.getAttribute("id"));
}
