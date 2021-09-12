// Copyright Golden Hammer Software
#pragma once

#include "GHString/GHIdentifier.h"

class GHFrameAnimSequence
{
public:
    GHFrameAnimSequence(void)
    :	mFirstFrame(0)
    ,	mEndFrame(1)
    ,	mLoopFrames(0)
    ,	mFramesPerSecond(20)
    {
    }
    
    // find the two border frames for a given time.
    // side effect: moves start time closer to current loop val
    //   otherwise anims will get slower and slower as time progresses.
	void findBorderFrames(float& seqStartTime, float time,
                          unsigned short& lowFrame, unsigned short& highFrame,
                          float& pctLow) const;

public:
	// index of the first frame.
	unsigned short mFirstFrame;
	// index of the final frame.
	unsigned short mEndFrame;
	// does the anim loop?
	int mLoopFrames;
	float mFramesPerSecond;
    GHIdentifier mId;
};
