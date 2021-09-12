// Copyright 2010 Golden Hammer Software
#include "GTSelectionHighlighter.h"
#include "GHGUIMgr.h"

constexpr static size_t kCategory = 0;

GTSelectionHighlighter::GTSelectionHighlighter(GHGUIMgr& guiMgr,
	GHGUIWidgetResource*& selectedWidget,
	GTUtil& util)
	: mGUIMgr(guiMgr)
	, mSelectedWidget(selectedWidget)
	, mUtil(util)
	, mIsActive(false)
{
	mHighlightWidget = mUtil.createPanel();

	GHPoint4 color(0, 0, 0, .1f);
	mUtil.setColor(*mHighlightWidget, color);

	mHighlightWidget->acquire();
}

GTSelectionHighlighter::~GTSelectionHighlighter(void)
{
	mHighlightWidget->release();
}

void GTSelectionHighlighter::update(void)
{
	if (mSelectedWidget)
	{
		const GHRect<float, 2>& screenRect = mSelectedWidget->get()->getScreenPos();

		GHGUIPosDesc posDesc;
		posDesc.mSize = screenRect.mMax;
		posDesc.mSize -= screenRect.mMin;
		posDesc.mOffset = screenRect.mMin;
		posDesc.mAlign.setCoords(0, 0);
		posDesc.mSizeAlign[0] = GHAlign::A_RIGHT;
		posDesc.mSizeAlign[1] = GHAlign::A_BOTTOM;
		mHighlightWidget->get()->setPosDesc(posDesc);
		mHighlightWidget->get()->updatePosition();

		add();
	}
	else
	{
		remove();
	}
}

void GTSelectionHighlighter::onActivate(void)
{

}

void GTSelectionHighlighter::onDeactivate(void)
{
	remove();
}

void GTSelectionHighlighter::add(void)
{
	if (!mIsActive)
	{
		GHGUITransitionDesc tDesc;
		mGUIMgr.pushWidget(mHighlightWidget, tDesc, 10.f, kCategory);
	}
	mIsActive = true;
}

void GTSelectionHighlighter::remove(void)
{
	if (mIsActive)
	{
		GHGUITransitionDesc tDesc;
		mGUIMgr.popWidget(mHighlightWidget, tDesc, kCategory, false);
	}
	mIsActive = false;
}
