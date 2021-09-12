// Copyright 2010 Golden Hammer Software
#include "GTInfoDisplayer.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GTIdentifiers.h"
#include "GTUtil.h"
#include "GHGUIText.h"

GTInfoDisplayer::GTInfoDisplayer(GHGUIWidgetResource*& currentPanel,
	GHPropertyContainer& properties, GTUtil& util)
	: mCurrentPanel(currentPanel)
	, mProperties(properties)
	, mUtil(util)
	, mWasDisplaying(false)
	, mWasClear(true)
{

}

void GTInfoDisplayer::update(void)
{
	if (!mWasDisplaying) {
		updateSelectionInfo();
		mWasDisplaying = true;
		return;
	}

	if (mCurrentPanel && mCurrentPanel->get())
	{
		const GHGUIPosDesc& currDesc = mCurrentPanel->get()->getPosDesc();
		if (mWasClear || currDesc != mLastPosDesc)
		{
			updateSelectionInfo();
			mLastPosDesc = currDesc;
		}
	}
	else
	{
		clearDisplay();
	}
}

void GTInfoDisplayer::onActivate(void)
{

}

void GTInfoDisplayer::onDeactivate(void)
{
	clearDisplay();
	mWasDisplaying = false;
}

void GTInfoDisplayer::clearDisplay(void)
{
	mProperties.setProperty(GTIdentifiers::GP_CURRSELECTEDNAME, "Nothing");
	mProperties.setProperty(GTIdentifiers::GP_CURRSELECTEDDESC, 0);
	mWasClear = true;
}

const char* toString(GHAlign::Enum val)
{
	switch (val)
	{
	case GHAlign::A_TOP:
		return "A_TOP";
	case GHAlign::A_BOTTOM:
		return "A_BOTTOM";
	case GHAlign::A_LEFT:
		return "A_LEFT";
	case GHAlign::A_RIGHT:
		return "A_RIGHT";
	case GHAlign::A_CENTER:
	default:
		return "A_CENTER";
	}
}

const char* toString(GHFillType::Enum val)
{
	return val == GHFillType::FT_PCT ? "FT_PCT" : "FT_PIXELS";
}

const char* toString(bool val)
{
	return val ? "true" : "false";
}


void GTInfoDisplayer::updateSelectionInfo(void)
{
	if (!mCurrentPanel || !mCurrentPanel->get()) {
		clearDisplay();
		return;
	}

	GHGUIWidget& panel = *mCurrentPanel->get();
	mProperties.setProperty(GTIdentifiers::GP_CURRSELECTEDNAME, panel.getId().getString());
	mProperties.setProperty(GTIdentifiers::GP_CURRSELECTEDDESC, mDescBuffer);

	const GHGUIPosDesc& pos = panel.getPosDesc();

	const char* hSizeAlign = toString(pos.mSizeAlign[0]);
	const char* vSizeAlign = toString(pos.mSizeAlign[1]);

	char textInfoBuffer[BUFSIZ] = "";
	const GHGUIWidgetResource* textChild = mUtil.getTextChild(*mCurrentPanel);
	if (textChild)
	{
		const GHGUIText* text = (const GHGUIText*)textChild->get();

		const char* hAlign = toString(text->getHAlign());
		const char* vAlign = toString(text->getVAlign());
		const char* fillType = toString(text->getHeightFillType());
		const char* wrap = toString(text->getWrap());

		snprintf(textInfoBuffer, BUFSIZ, "tHeight=%f tFill=%s tXAlign=%s tYAlign=%s tWrap=%s", text->getTextHeight(), fillType, hAlign, vAlign, wrap);
	}

	snprintf(mDescBuffer, BUFSIZ,
		"xFill=%s yFill=%s align=\"%f %f\" offset=\"%f %f\" size=\"%f %f\" xSizeAlign=%s ySizeAlign=%s %s",
		toString(pos.mXFill),
		toString(pos.mYFill),
		pos.mAlign[0], pos.mAlign[1],
		pos.mOffset[0], pos.mOffset[1],
		pos.mSize[0], pos.mSize[1],
		hSizeAlign, vSizeAlign,
		textInfoBuffer);
}
