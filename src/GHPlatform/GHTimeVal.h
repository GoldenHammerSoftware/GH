// Copyright Golden Hammer Software
#pragma once

class GHTimeCalculator;

// system-independent class for holding time information.
class GHTimeVal
{
public:
	GHTimeVal(void);

	// move the current frame info into the previous frame.
	// setTime to newTime.
	// calcTimePassed.
	void newFrame(float newTime);

	float getTime(void) const { return mTime; }

	float getLastTime(void) const { return mLastTime; }
	void setLastTime(float nrt) { mLastTime = nrt; }

	float getTimePassed(void) const { return mTimePassed; }

	// make this val current with 0 timepassed
	void clearTimePassed(const GHTimeCalculator& timeCalculator);

private:
	void setTime(float nrt) { mTime = nrt; }
	void setTimePassed(float time) { mTimePassed = time; }

	// calc the new normalized time passed assuming the current values are valid.
	void calcTimePassed(void);

private:
	// ** this frame **
	// normalized curr time.
	float mTime;

	// ** previous frame, for finding time passed **
	// normalized last frame time.
	float mLastTime;

	// ** timepassed **
	float mTimePassed;
};
