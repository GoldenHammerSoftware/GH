#include <assert.h>
#include <screen/screen.h>
#include <bps/sensor.h>
#include <bps/navigator.h>
#include <bps/screen.h>
#include <bps/bps.h>
#include <bps/event.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <EGL/egl.h>
#include <GLES/gl.h>
#include "bbutil.h"

#include "GHBBSystemServices.h"
#include "GHInputState.h"
#include "GHMessageQueue.h"
#include "GHBBRenderServices.h"
#include "GHBBGameServices.h"
#include "GHNullIAPStoreFactory.h"
#include "GHMutex.h"
#include "GHThreadFactory.h"
#include "GHAppRunner.h"
#include "GHBowlingApp.h"
#include "GHBBSystemEventHandler.h"
#include "GHBBScoreloopStatTrackerFactory.h"
#include "GHNullMultiplayerFactory.h"

#include "GHMessage.h"
#include "GHMessageTypes.h"
#include "GHBaseIdentifiers.h"
#include "GHBowlingEntityProperties.h"
#include "GHBBEventHandler.h"

#include "GHBBGLESContext.h"

int main(int argc, char *argv[])
{
	//Create a screen context that will be used to create an EGL surface to to receive libscreen events
	static screen_context_t screen_cxt;
	screen_create_context(&screen_cxt, 0);
    //Initialize BPS library
    bps_initialize();

    GHBBSystemServices* systemServices = new GHBBSystemServices();
    GHInputState* volatileInputState = new GHInputState(10,1,1);
    GHMessageQueue* appMessageQueue = new GHMessageQueue(systemServices->mThreadFactory);
    GHBBRenderServices* renderServices = new GHBBRenderServices(*systemServices, systemServices->mHashTable, screen_cxt);
    GHBBGameServices* gameServices = new GHBBGameServices(*systemServices, *renderServices, *appMessageQueue);
    GHIAPStoreFactory* iapFactory = new GHNullIAPStoreFactory(true);
    GHMutex* inputMutex = systemServices->mThreadFactory->createMutex();

    GHBBEventHandlerList bbEventHandlerList;

    //Signal BPS library that navigator and screen events will be requested
    if (BPS_SUCCESS != screen_request_events(screen_cxt)) {
        fprintf(stderr, "screen_request_events failed\n");
        screen_destroy_context(screen_cxt);
        return 0;
    }
    if (BPS_SUCCESS != navigator_request_events(0)) {
        fprintf(stderr, "navigator_request_events failed\n");
        screen_destroy_context(screen_cxt);
        return 0;
    }
    //Signal BPS library that navigator orientation is not to be locked
    if (BPS_SUCCESS != navigator_rotation_lock(false)) {
        fprintf(stderr, "navigator_rotation_lock failed\n");
        screen_destroy_context(screen_cxt);
        return 0;
    }

    GHControllerMgr osFrameControllerMgr;

    GHBBScoreloopStatTrackerFactory statTrackerFactory(osFrameControllerMgr,
    												   *systemServices->mThreadFactory,
    												   systemServices->mXMLSerializer,
    												   bbEventHandlerList,
    												   gameServices->mScoreloopScoresData,
    												   gameServices->mAchievementsData,
    												   "dcd60462-8f8b-4233-9b0d-0e38d240973b", //Game ID
    												   "QQSOE3EMxoXuQzoOWV1bJOrlzHZXUzCVHx/MBITfCGfXUlQ/5DipdQ==", //Game Secret
    												   "1.1", //version
    												   "HTQ", //Game Currency code
    												   "en"); //language
    GHNullMultiplayerFactory mpFactory;

    GHBowlingApp* app = new GHBowlingApp(*systemServices, *renderServices, *gameServices, *appMessageQueue,
    		*iapFactory, statTrackerFactory, mpFactory, 0, false);

    GHMessage iapPropMessage(GHMessageTypes::SETAPPPROPERTY);
    iapPropMessage.getPayload().setProperty(GHBaseIdentifiers::PROPID,
    		GHProperty(&GHBowlingEntityProperties::GP_IAPPURCHASED));
    iapPropMessage.getPayload().setProperty(GHBaseIdentifiers::PROPVAL, true);
    appMessageQueue->handleMessage(iapPropMessage);

	GHBBSystemEventHandler bbSystemEventHandler(*volatileInputState, *appMessageQueue,
			renderServices->getScreenInfo(), true, renderServices->getContext());

	GHAppRunner* appRunner = new GHAppRunner(app, systemServices->mEventMgr, *appMessageQueue,
            *systemServices->mInputState, *volatileInputState, *inputMutex,
            *systemServices->mThreadFactory, 0);

	// we need to draw one empty frame or we hang on launch.
    renderServices->getContext().beginFrame();
    renderServices->getContext().endFrame();

    bool exit_application = false;
    while (!exit_application)
    {
        //Request and process all available BPS events
        bps_event_t *event = NULL;

        for(;;) {
            int rc = bps_get_event(&event, 0);
            assert(rc == BPS_SUCCESS);
			if (rc != BPS_SUCCESS)
			{
				break;
			}

            int domain = 0;

            if (event)
            {
                domain = bps_event_get_domain(event);

                bool handled = false;
                size_t numEventHandlers = bbEventHandlerList.size();
                for (size_t i = 0; i < numEventHandlers; ++i)
                {
                	GHBBEventHandler* handler = bbEventHandlerList[i];
                	if (handler->handleBBEvent(event, domain))
                	{
                		handled = true;
                		break;
                	}
                }

                if (!handled)
                {
                	if (domain == navigator_get_domain())
                	{
                		if (NAVIGATOR_EXIT == bps_event_get_code(event))
                	    {
                	    	exit_application = true;
                	        // todo?
                	        //appLauncher->getInterruptManager().handleTerminationInterrupt();
                	    }
                	    else
                	    {
                	        bbSystemEventHandler.handleNavigatorEvent(event);
                	    }
                	}
                	else if (!bbSystemEventHandler.isPaused())
                	{
                		if (domain == screen_get_domain())
                	    {
                			bbSystemEventHandler.handleScreenEvent(event);
                	    }
                	    else if (domain == sensor_get_domain())
                	    {
                	        bbSystemEventHandler.handleSensorEvent(event);
                	    }
                	}
                }

            } else {
                break;
            }
        }

        if (!exit_application && !bbSystemEventHandler.isPaused())
        {
        	appRunner->runFrame();
        	osFrameControllerMgr.update();
        }
    }

    delete appRunner;
    delete app;
    delete inputMutex;
    delete iapFactory;
    delete gameServices;
    delete renderServices;
    delete appMessageQueue;
    delete volatileInputState;
    delete systemServices;

    //Stop requesting events from libscreen
    screen_stop_events(screen_cxt);
    //Shut down BPS library for this process
    bps_shutdown();

    //Destroy libscreen context
    screen_destroy_context(screen_cxt);
    return 0;
}
