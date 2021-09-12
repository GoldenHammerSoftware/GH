// Copyright Golden Hammer Software
#pragma once

class GHTimeVal;

/// Platform interface for updating frame time.
class GHTimeCalculator
{
public:
	virtual ~GHTimeCalculator(void) {}
	
	// Grab the current time in the most precise and cheap way possible.
	virtual float calcTime(void) const = 0;
};
