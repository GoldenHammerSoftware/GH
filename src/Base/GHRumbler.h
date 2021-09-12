// Copyright Golden Hammer Software
#pragma once

//Interface for triggering vibrate/rumble in controllers
class GHRumbler
{
public:
	virtual ~GHRumbler(void) { }

	//left- and right rumble should be passed values 0-1, where 1 is max rumble
	virtual void setRumble(int controllerIndex, float leftRumble, float rightRumble) = 0;
};
