// Copyright Golden Hammer Software
#include "GHNewsGUITransitionXMLLoader.h"
#include "GHNewsGUITransition.h"
#include "GHGUIProperties.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHPlatform/GHDebugMessage.h"

GHNewsGUITransitionXMLLoader::GHNewsGUITransitionXMLLoader(GHNewsMgr& news)
: mNews(news)
{
}

void* GHNewsGUITransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidget* parentWidget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);
	if (!parentWidget) {
		GHDebugMessage::outputString("Failed to find parent widget for news.");
		return 0;
	}
	return new GHNewsGUITransition(*parentWidget, mNews);
}
