#pragma once

#include "GHUtils/GHController.h"
#include "GHInputState.h"
#include <list>

class GHTimeVal;
class GHInputState;

//A record of what happened to the input state over a period of time.
// May be recorded by the GHInputStateRecorder or may be loaded from file.
// Up to the application to decide what to do with it.
// May be fed into another GHInputState
struct GHInputStateRecording
{
	struct Keyframe
	{
		GHInputStructs::InputEventList mInputEventList;
		float mTimestamp;
	};

	std::list<Keyframe> mKeyframes;
};

//Records a GHInputStateRecording. May be used as a controller
// or may also have recording be controlled by an external class.
class GHInputStateRecorder : public GHController
{
public:
	GHInputStateRecorder(const GHTimeVal& timeVal, 
						 const GHInputState& inputState,
						 size_t maxFramesToRecord=~size_t(0));

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	void startRecording(void);
	void pauseRecording(void);
	void clearRecording(GHInputStateRecording* recording = nullptr);
	void recordCurrentFrame(GHInputStateRecording* recording = nullptr);

	const GHInputStateRecording& getRecording(void) const { return mRecording; }

private:
	GHInputStateRecording mRecording;
	const GHTimeVal& mTimeVal;
	const GHInputState& mInputState;
	float mStartTime{ 0 };
	float mPauseStartTime{ 0 };
	size_t mMaxFramesToRecord{ ~size_t(0) };
	bool mIsPaused{ false };
};

