// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHUtils/GHController.h"
#include "GHGUIWidget.h"
#include "GTUtil.h"

class GHGUIMgr;

class GTSelectionHighlighter : public GHController
{
public:
	GTSelectionHighlighter(GHGUIMgr& guiMgr,
						   GHGUIWidgetResource*& selectedWidget,
						   GTUtil& util);
	~GTSelectionHighlighter(void);
	
	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

private:
	void add(void);
	void remove(void);

private:
	GHGUIMgr& mGUIMgr;
	GHGUIWidgetResource*& mSelectedWidget;
	GHGUIWidgetResource* mHighlightWidget;
	GTUtil& mUtil;
	bool mIsActive;
};
