// Copyright 2010 Golden Hammer Software
#include "GHMetroURLOpenerTransitionLoader.h"
#include "GHXMLNode.h"
#include "GHMetroURLOpenerTransition.h"

void* GHMetroURLOpenerTransitionLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const char* url = node.getAttribute("url");
	return new GHMetroURLOpenerTransition(url);
}

