// Copyright Golden Hammer Software
#include "GHWin32URLOpenerTransitionXMLLoader.h"
#include "GHWin32URLOpenerTransition.h"
#include "Base/GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"

GHWin32URLOpenerTransitionXMLLoader::GHWin32URLOpenerTransitionXMLLoader(void)
{
}

void* GHWin32URLOpenerTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	const char* url = node.getAttribute("url");
	if (!url) {
		GHDebugMessage::outputString("No url found for url opener.");
	}
	return new GHWin32URLOpenerTransition(url);
}
