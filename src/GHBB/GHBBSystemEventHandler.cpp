#include "GHBBSystemEventHandler.h"
#include "GHInputState.h"
#include "GHDebugMessage.h"
#include "GHMessageHandler.h"
#include "GHMessage.h"
#include "GHMessageTypes.h"
#include "GHKeyDef.h"
#include "GHFloat.h"
#include "GHScreenInfo.h"
#include "GHBBGLESContext.h"

#include <bps/sensor.h>
#include <bps/screen.h>
#include <bps/navigator.h>
#include <bps/orientation.h>
#include <stdlib.h>
#include <bps/deviceinfo.h>

GHBBSystemEventHandler::GHBBSystemEventHandler(GHInputState& inputState,
												GHMessageHandler& appMessageHandler,
												GHScreenInfo& screenInfo,
												bool allowsRotate,
												GHBBGLESContext& context)
: mInputState(inputState)
, mAppMessageHandler(appMessageHandler)
, mIsPaused(false)
, mAccelSwapped(true)
, mScreenInfo(screenInfo)
, mAllowsRotate(allowsRotate)
, mContext(context)
{
	bool isPlaybook = false;
	if (sensor_is_supported(SENSOR_TYPE_ACCELEROMETER))
	{
		///*
		// tablet 2.1 sdk version
		deviceinfo_data_t deviceDetails;
		int getDetailsRet = deviceinfo_get_data(&deviceDetails);
		if (getDetailsRet == BPS_FAILURE) mAccelSwapped = true;
		else
		{
			if (strstr(deviceDetails.device_os, "PlayBook")) isPlaybook = true;
			if (deviceDetails.hardware_id == 100669958) isPlaybook = true;
			if (isPlaybook)
			{
				mAccelSwapped = false;
			}
			deviceinfo_free_data(&deviceDetails);
		}
		//*/

		/*
		// bb10 sdk version
		deviceinfo_details_t *deviceDetails;
		int getDetailsRet = deviceinfo_get_details(&deviceDetails);
		const char* hardwareid = deviceinfo_details_get_hardware_id(deviceDetails);
		if (!strcmp(hardwareid, "100669958")) {
			isPlaybook = true;
		}
		else {
			const char* osString = deviceinfo_details_get_device_os(deviceDetails);
			if (strstr(osString, "PlayBook")) {
				isPlaybook = true;
			}
		}
		if (isPlaybook)
		{
			mAccelSwapped = false;
		}
		deviceinfo_free_details(&deviceDetails);
		*/

		static const int ACCELEROMETER_RATE = 25000;
		sensor_set_rate(SENSOR_TYPE_ACCELEROMETER, ACCELEROMETER_RATE);
		sensor_set_skip_duplicates(SENSOR_TYPE_ACCELEROMETER, true);
		sensor_request_events(SENSOR_TYPE_ACCELEROMETER);
	}
	else
	{
	    GHDebugMessage::outputString("Acceleration values not supported by device!");
	}

	orientation_direction_t direction;
	int angle;
	orientation_get(&direction, &angle);
	if (angle == 180 && isPlaybook) {
		calculateAccelForceMultiplier(1.f);
	}
	else if (angle == 90 && !isPlaybook) {
		calculateAccelForceMultiplier(1.f);
	}
	else {
		calculateAccelForceMultiplier(-1.f);
	}
}

void GHBBSystemEventHandler::handleSensorEvent(bps_event_t* event)
{
	if (SENSOR_ACCELEROMETER_READING == bps_event_get_code(event))
	{
		GHPoint3 force;
		sensor_event_get_xyz(event,&force[0], &force[1], &force[2]);
		if (mAccelSwapped)
		{
			float dummy = force[0];
			force[0] = force[1];
			force[1] = -1.0f * dummy;
		}
		force *= mAccelForceMultiplier;

		mInputState.handleAccelerometer(0, force);
	}
}

void GHBBSystemEventHandler::handleScreenEvent(bps_event_t *event)
{
    screen_event_t screen_event = screen_event_get_event(event);

    int screen_val;
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TYPE, &screen_val);
    int screen_coords[2];
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_SOURCE_POSITION, screen_coords);
    int touchIndex;
    screen_get_event_property_iv(screen_event, SCREEN_PROPERTY_TOUCH_ID, &touchIndex);

    GHPoint2 normPos;
    for (int i = 0; i < 2; ++i)
    {
    	normPos[i] = ((float)screen_coords[i]) / ((float)mScreenInfo.getSize()[i]);
    }

    switch (screen_val)
    {
    case SCREEN_EVENT_MTOUCH_TOUCH:
    {
    	mInputState.handlePointerActive(touchIndex, true);
    	int mouseKey = (int)GHKeyDef::KEY_MOUSE1 + (touchIndex*mInputState.getNumMouseButtons());
    	mInputState.handleKeyChange(0, mouseKey, true);
    	mInputState.handlePointerChange(touchIndex, normPos);
    	break;
    }
    case SCREEN_EVENT_MTOUCH_MOVE:
    {
    	mInputState.handlePointerChange(touchIndex, normPos);
    	break;
    }
    case SCREEN_EVENT_MTOUCH_RELEASE:
    {
    	int mouseKey = (int)GHKeyDef::KEY_MOUSE1 + (touchIndex*mInputState.getNumMouseButtons());
    	mInputState.handleKeyChange(0, mouseKey, false);
    	mInputState.handlePointerChange(touchIndex, GHPoint2(-1, -1));
    	mInputState.handlePointerActive(touchIndex, false);
        break;
    }
    }
}

void GHBBSystemEventHandler::handleNavigatorEvent(bps_event_t* event)
{
	switch(bps_event_get_code(event))
	{
	case NAVIGATOR_WINDOW_STATE:
	{
		navigator_window_state_t currentState = navigator_event_get_window_state(event);
		if (currentState == NAVIGATOR_WINDOW_FULLSCREEN)
		{
			if (mIsPaused)
			{
				mAppMessageHandler.handleMessage(GHMessage(GHMessageTypes::UNPAUSEINTERRUPT));
				mIsPaused = false;
			}
		}
		else if (!mIsPaused)
		{
			mAppMessageHandler.handleMessage(GHMessage(GHMessageTypes::PAUSEINTERRUPT));
			mIsPaused = true;
		}
		break;
	}
	case NAVIGATOR_ORIENTATION_CHECK:
	{
		if (mAllowsRotate)
		{
			navigator_orientation_check_response(event, true);
		}
		else
		{
			int angle = navigator_event_get_orientation_angle(event);
			bool willRotate = angle == 0;
			navigator_orientation_check_response(event, willRotate);
		}
		break;
	}
	case NAVIGATOR_ORIENTATION:
	{
		int angle = navigator_event_get_orientation_angle(event);

		mContext.handleScreenRotation(angle);
		int screenWidth, screenHeight;
		mContext.getScreenSize(screenWidth, screenHeight);
	    mScreenInfo.setSize(GHPoint2i(screenWidth, screenHeight));
		mScreenInfo.setPixelSizeFactor(1/800.0f, 0.0f);

        GHMessage resizeMessage(GHMessageTypes::WINDOWRESIZE);
        mAppMessageHandler.handleMessage(resizeMessage);

		navigator_done_orientation(event);
		break;
	}
	}
}

void GHBBSystemEventHandler::calculateAccelForceMultiplier(float dir)
{
	if (GHFloat::isZero(SENSOR_GRAVITY_EARTH))
	{
		mAccelForceMultiplier = 1.f;
	}
	else
	{
		mAccelForceMultiplier = 1.f/SENSOR_GRAVITY_EARTH;
	}

	//The numbers come in reversed from what we expect in YrgEngine
	mAccelForceMultiplier *= dir;
}

