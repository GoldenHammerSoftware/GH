// Copyright Golden Hammer Software
#include "GHNewsGUITransition.h"
#include "GHNewsMgr.h"

GHNewsGUITransition::GHNewsGUITransition(GHGUIWidget& parent, GHNewsMgr& news)
: mParent(parent)
, mNews(news)
{
}

void GHNewsGUITransition::activate(void)
{
	mNews.addNewsToGUI(mParent);
}

void GHNewsGUITransition::deactivate(void)
{
	mNews.removeNewsFromGUI(mParent);
}
