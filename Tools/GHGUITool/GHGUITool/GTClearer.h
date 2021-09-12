// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHUtils/GHMessageHandler.h"
#include "GHGUIWidget.h"

class GTClearer : public GHMessageHandler
{
public:
	GTClearer(GHGUIWidgetResource& topLevelGUI, GHGUIWidgetResource*& currentSelection);
	virtual ~GTClearer(void);

	virtual void handleMessage(const GHMessage& message);

private:
	GHGUIWidgetResource& mTopLevelGUI;
	GHGUIWidgetResource*& mCurrentSelection;
};
