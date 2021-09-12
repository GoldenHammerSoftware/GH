// Copyright Golden Hammer Software
#include "GHGUISizeOscillater.h"
#include "GHGUIWidget.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHMath/GHPi.h"

GHGUISizeOscillater::GHGUISizeOscillater(GHGUIWidget& widget, const GHTimeVal& time, float changeMax, float changeSpeed)
: mWidget(widget)
, mTime(time)
, mChangeMax(changeMax)
, mChangeSpeed(changeSpeed)
, mStartTime(0.0f)
{
}

void GHGUISizeOscillater::update(void)
{
	float timeDist = mTime.getTime() - mStartTime;
	float changeMult = sin(timeDist*mChangeSpeed - GHPI / 2.0f);
	changeMult = (changeMult+1.0f)/2.0f;
	float changeAmt = changeMult*mChangeMax;

	GHGUIPosDesc posDesc = mWidget.getPosDesc();
	posDesc.mSize[0] = mBaseSize[0] * (1.0f+changeAmt);
	posDesc.mSize[1] = mBaseSize[1] * (1.0f + changeAmt);
	mWidget.setPosDesc(posDesc);
	mWidget.updatePosition();
}

void GHGUISizeOscillater::onActivate(void)
{
	mStartTime = mTime.getTime();
	const GHGUIPosDesc& posDesc = mWidget.getPosDesc();
	mBaseSize = posDesc.mSize;
}

void GHGUISizeOscillater::onDeactivate(void)
{
	GHGUIPosDesc posDesc = mWidget.getPosDesc();
	posDesc.mSize = mBaseSize;
	mWidget.setPosDesc(posDesc);
	mWidget.updatePosition();
}
