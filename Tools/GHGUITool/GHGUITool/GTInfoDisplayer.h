// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include "GHGUIWidget.h"
#include "GHGUIPosDesc.h"

class GHPropertyContainer;
class GTUtil;

class GTInfoDisplayer : public GHController
{
public:
	GTInfoDisplayer(GHGUIWidgetResource*& currentPanel, 
					GHPropertyContainer& properties,
					GTUtil& util);

	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	void clearDisplay(void);
	void updateSelectionInfo(void);

private:
	GHGUIWidgetResource*& mCurrentPanel;
	GHPropertyContainer& mProperties;
	GTUtil& mUtil;

	GHGUIPosDesc mLastPosDesc;
	bool mWasDisplaying;
	bool mWasClear = true;
	const static int BUFSZ = 512;
	char mDescBuffer[BUFSZ];
};