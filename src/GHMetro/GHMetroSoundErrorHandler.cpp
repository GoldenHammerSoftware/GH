#include "GHMetroSoundErrorHandler.h"
#include "GHMetroSoundDevice.h"
#include "GHUtils/GHControllerMgr.h"
#include "GHUtils/GHMessage.h"
#include "Base/GHBaseIdentifiers.h"

GHMetroSoundErrorHandler::GHMetroSoundErrorHandler(GHMetroSoundDevice& device,
											   GHControllerMgr& controllerManager,
											   const GHTimeCalculator& time,
											   GHMessageHandler& eventHandler)
: mDevice(device)
, mControllerManager(controllerManager)
, mErrorHappened(false)
, mExceptionsAllowed(false)
, mTimeOfLastError(-1)
, mTimeCalculator(time)
, mEventHandler(eventHandler)
{
	mControllerManager.add(this);
}

GHMetroSoundErrorHandler::~GHMetroSoundErrorHandler(void)
{
	mControllerManager.remove(this);
}

void GHMetroSoundErrorHandler::update(void)
{
	if (mErrorHappened
		&& mTimeCalculator.calcTime() - mTimeOfLastError >= 1.f) //arbitrary timeout before handling the error
	{
		mExceptionsAllowed = true;
		try
		{
			mDevice.shutdown();
			mDevice.init();
			mErrorHappened = false;
		}
		catch (...)
		{
		}
		mExceptionsAllowed = false;
		mEventHandler.handleMessage(GHMessage(GHBaseIdentifiers::M_AUDIODEVICERESET));
	}
}

void GHMetroSoundErrorHandler::onActivate(void)
{
}

void GHMetroSoundErrorHandler::onDeactivate(void)
{
}

void GHMetroSoundErrorHandler::checkResult(HRESULT result, const char* methodName)
{
	if (FAILED(result))
	{
		GHDebugMessage::outputString("%s failed with result 0x%X", methodName ? methodName : "Some IXAudio2 call", result);
		onCriticalError();
		throw result;
	}
}

void GHMetroSoundErrorHandler::onCriticalError(void)
{
	mErrorHappened = true;
	mTimeOfLastError = mTimeCalculator.calcTime();
}
