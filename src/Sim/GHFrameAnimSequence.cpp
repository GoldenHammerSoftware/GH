// Copyright Golden Hammer Software
#include "GHFrameAnimSequence.h"
#include <math.h>

void GHFrameAnimSequence::findBorderFrames(float& seqStartTime, float time,
                                           unsigned short& lowFrame, unsigned short& highFrame,
                                           float& pctLow) const
{
    //GHDebugMessage::outputString("Finding frames startTime=%f", mSequenceStartTime);
    
	float timeSinceStart = time - seqStartTime;

	// loop around animations that need it.
	if (mLoopFrames > 0) {
        bool wrapped = false;
		float seqTime = (mEndFrame-mFirstFrame+1)/mFramesPerSecond;
		float seqWrapTime = (mLoopFrames+1)/mFramesPerSecond;
		while (timeSinceStart < 0) {
			timeSinceStart += seqWrapTime;
		}
		while (timeSinceStart > seqTime) {
            // todo: make this more efficient for long-running animations.
			timeSinceStart -= seqWrapTime;
			wrapped = true;
		}
		if (wrapped) {
			seqStartTime = time - timeSinceStart;
		}
	}
	else if (timeSinceStart < 0) { //May also want to wrap backwards in this case?
		timeSinceStart = 0;
	}

	float currFrameF = mFirstFrame + mFramesPerSecond*timeSinceStart;
	if (currFrameF > mEndFrame) {
		if (!mLoopFrames) {
			lowFrame = mEndFrame;
			highFrame = mEndFrame;
			pctLow = 1.0f;
			return;
		}
	}

	lowFrame = (unsigned short)(::floor(currFrameF));
	if (mLoopFrames && lowFrame == mEndFrame) {
		highFrame = mEndFrame - mLoopFrames;
	}
	else {
		highFrame = lowFrame + 1;
	}
	pctLow = 1.0f - (currFrameF - lowFrame);
}
