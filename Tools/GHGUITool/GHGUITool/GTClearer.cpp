// Copyright 2010 Golden Hammer Software
#include "GTClearer.h"

GTClearer::GTClearer(GHGUIWidgetResource& topLevelGUI, GHGUIWidgetResource*& currentSelection)
	: mTopLevelGUI(topLevelGUI)
	, mCurrentSelection(currentSelection)
{

}

GTClearer::~GTClearer(void)
{

}

void GTClearer::handleMessage(const GHMessage& message)
{
	mCurrentSelection = 0;

	while (mTopLevelGUI.get()->getNumChildren())
	{
		mTopLevelGUI.get()->removeChild(mTopLevelGUI.get()->getChildAtIndex(0));
	}
}
