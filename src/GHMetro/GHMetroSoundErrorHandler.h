#pragma once
#include "GHUtils/GHController.h"
#include "GHXAudio2Include.h"

class GHMetroSoundDevice;
class GHControllerMgr;
class GHTimeCalculator;
class GHMessageHandler;

class GHMetroSoundErrorHandler : public GHController
{
public:
	GHMetroSoundErrorHandler(GHMetroSoundDevice& device,
							 GHControllerMgr& controllerManager,
							 const GHTimeCalculator& time,
							 GHMessageHandler& eventHandler);
	~GHMetroSoundErrorHandler(void);
	virtual void update(void);
	virtual void onActivate(void);
	virtual void onDeactivate(void);

	void onCriticalError(void);

	void checkResult(HRESULT result, const char* methodName = 0);

private:
	const GHTimeCalculator& mTimeCalculator;
	GHMetroSoundDevice& mDevice;
	GHControllerMgr& mControllerManager;
	GHMessageHandler& mEventHandler;
	bool mErrorHappened;
	bool mExceptionsAllowed;
	float mTimeOfLastError;
};