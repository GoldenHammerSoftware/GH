// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHController.h"
#include <vector>
#include <list>
#include "GHString/GHIdentifier.h"

class GHPropertyContainer;
class GHTransition;

//Intended to be used with slider bars that have a finite number of specific notches
//	-Monitors a property value
//	-forces that value to be on its list of notches
//	-has a transition list for each that triggers when that notch is switched to from another notch
class GHGUINotchedSliderMonitor : public GHController
{
public:
	GHGUINotchedSliderMonitor(GHPropertyContainer& properties, const GHIdentifier& propId);
	~GHGUINotchedSliderMonitor(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	struct Notch
	{
		float mNotchValue{ 0 };
		std::vector<GHTransition*> mTransitions;
	};
	//for loading. Creates a notch with specified value and an empty transition list. Returns the notch so the transition list can be filled
	Notch& createNotch(float notchValue);

private:
	Notch& selectClosestValue(float value);
	void triggerChange(Notch& notch);

private:
	GHPropertyContainer& mProperties;
	GHIdentifier mPropId;
	typedef std::list<Notch> NotchList;
	NotchList mNotches;
	Notch* mPreviousNotch{ 0 };
};

