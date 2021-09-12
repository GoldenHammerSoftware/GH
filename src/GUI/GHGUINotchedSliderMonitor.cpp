// Copyright Golden Hammer Software
#include "GHGUINotchedSliderMonitor.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHUtils/GHTransition.h"
#include <math.h>

GHGUINotchedSliderMonitor::GHGUINotchedSliderMonitor(GHPropertyContainer& properties, const GHIdentifier& propId)
	: mProperties(properties)
	, mPropId(propId)
	, mPreviousNotch(0)
{

}

GHGUINotchedSliderMonitor::~GHGUINotchedSliderMonitor(void)
{
	NotchList::iterator iter = mNotches.begin(), iterEnd = mNotches.end();
	for (; iter != iterEnd; ++iter)
	{
		Notch& notch = *iter;
		size_t numTransitions = notch.mTransitions.size();
		for (size_t t = 0; t < numTransitions; ++t)
		{
			delete notch.mTransitions[t];
		}
	}
}

void GHGUINotchedSliderMonitor::onActivate(void)
{

}

void GHGUINotchedSliderMonitor::onDeactivate(void)
{

}

void GHGUINotchedSliderMonitor::update(void)
{
	if (!mNotches.size()) { return;  }

	float currentValue = mProperties.getProperty(mPropId);
	Notch& notch = selectClosestValue(currentValue);
	if (&notch != mPreviousNotch)
	{
		triggerChange(notch);
		mPreviousNotch = &notch;
	}
	mProperties.setProperty(mPropId, notch.mNotchValue);
}

GHGUINotchedSliderMonitor::Notch& GHGUINotchedSliderMonitor::createNotch(float notchValue)
{
	mNotches.push_back(Notch());
	Notch& ret = mNotches.back();
	ret.mNotchValue = notchValue;
	return ret;
}

GHGUINotchedSliderMonitor::Notch& GHGUINotchedSliderMonitor::selectClosestValue(float value)
{
	NotchList::iterator iter = mNotches.begin(), iterEnd = mNotches.end();
	NotchList::iterator selected = iter;
	for (; iter != iterEnd; ++iter)
	{
		Notch& curr = *iter;
		Notch& bestPick = *selected;

		float currDiff = (float)fabs(value - curr.mNotchValue);
		float bestDiff = (float)fabs(value - bestPick.mNotchValue);

		if (currDiff < bestDiff)
		{
			selected = iter;
		}
	}
	return *selected;
}

void GHGUINotchedSliderMonitor::triggerChange(Notch& notch)
{
	size_t numTransitions = notch.mTransitions.size();
	for (size_t i = 0; i < numTransitions; ++i)
	{
		GHTransition* transition = notch.mTransitions[i];
		transition->activate();
	}
	for (int i = ((int)numTransitions) - 1; i >= 0; --i)
	{
		GHTransition* transition = notch.mTransitions[i];
		transition->deactivate();
	}
}

