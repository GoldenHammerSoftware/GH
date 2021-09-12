#include "GHInputStateRecorder.h"
#include "GHPlatform/GHTimeVal.h"
#include "GHInputState.h"

GHInputStateRecorder::GHInputStateRecorder(const GHTimeVal& timeVal, const GHInputState& inputState, size_t maxFramesToRecord)
	: mTimeVal(timeVal)
	, mInputState(inputState)
	, mMaxFramesToRecord(maxFramesToRecord)
{

}

void GHInputStateRecorder::onActivate(void)
{
	mIsPaused = false;
	clearRecording();
	startRecording();
}

void GHInputStateRecorder::onDeactivate(void)
{
	pauseRecording();
}

void GHInputStateRecorder::update(void)
{
	recordCurrentFrame();
}

void GHInputStateRecorder::startRecording(void)
{
	mStartTime = mTimeVal.getTime();
	if (mIsPaused)
	{
		mStartTime -= mPauseStartTime;
	}
	mIsPaused = false;
}

void GHInputStateRecorder::pauseRecording(void)
{
	if (!mIsPaused)
	{
		mPauseStartTime = mTimeVal.getTime() - mStartTime;
		mIsPaused = true;
	}
}

void GHInputStateRecorder::clearRecording(GHInputStateRecording* recording)
{
	if (!recording) {
		recording = &mRecording;
	}
	
	recording->mKeyframes.clear();
}

void GHInputStateRecorder::recordCurrentFrame(GHInputStateRecording* recording)
{
	if (!recording) {
		recording = &mRecording;
	}

	if (recording->mKeyframes.size() >= mMaxFramesToRecord) {
		return;
	}

	const GHInputStructs::InputEventList& events = mInputState.getInputEvents();
	if (events.size())
	{
		recording->mKeyframes.push_back(GHInputStateRecording::Keyframe());
		auto& keyframe = recording->mKeyframes.back();
		keyframe.mTimestamp = mTimeVal.getTime() - mStartTime;
		keyframe.mInputEventList = events;
	}
}
