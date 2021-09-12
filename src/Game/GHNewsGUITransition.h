// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHTransition.h"

class GHGUIWidget;
class GHNewsMgr;

// transition for fetching and displaying news items on a gui panel.
class GHNewsGUITransition : public GHTransition
{
public:
	GHNewsGUITransition(GHGUIWidget& parent, GHNewsMgr& news);

	virtual void activate(void);
	virtual void deactivate(void);

private:
	GHGUIWidget& mParent;
	GHNewsMgr& mNews;
};
