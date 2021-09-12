#include "GHBaseXMLInitializer.h"
#include "GHAppShard.h"
#include "GHSystemServices.h"

#include "GHPropertyContainerXMLLoader.h"
#include "GHResourceLoaderXML.h"
#include "GHXMLLoaderResource.h"
#include "GHStateMachineXMLLoader.h"
#include "GHXMLRedirectLoader.h"
#include "GHPropertyStringGeneratorXMLLoader.h"
#include "GHMessageSenderTransitionXMLLoader.h"
#include "GHMessageHandlerTransitionXMLLoader.h"
#include "GHSoundTransitionXMLLoader.h"
#include "GHSoundProfileTransitionXMLLoader.h"
#include "GHPropertyToggleTransitionXMLLoader.h"
#include "GHControllerTransitionXMLLoader.h"
#include "GHPropertyCopyTransitionXMLLoader.h"
#include "GHTransitionSwitchXMLLoader.h"
#include "GHStateChangeTransitionXMLLoader.h"
#include "GHDebugTransitionXMLLoader.h"
#include "GHTransitionListTransitionXMLLoader.h"
#include "GHTransitionMessageHandlerXMLLoader.h"
#include "GHMessageXMLLoader.h"
#include "GHMessageTypes.h"
#include "GHPropertyXMLInitializer.h"

void GHBaseXMLInitializer::initXMLLoaders(GHAppShard& appShard, GHSystemServices& systemServices)
{
    GHPropertyXMLInitializer propInit;
    propInit.initAppShard(appShard, systemServices);
    GHPropertyContainerXMLLoader* propsLoader = (GHPropertyContainerXMLLoader*)appShard.mXMLObjFactory.getLoader("properties");
    assert(propsLoader);

    GHResourceLoaderXML* xmlResLoader = new GHResourceLoaderXML(appShard.mXMLObjFactory);
    appShard.mResourceFactory.addLoader(xmlResLoader, 1, ".xml");
    GHXMLLoaderResource* resXMLLoader = new GHXMLLoaderResource(appShard.mResourceFactory);
    appShard.mXMLObjFactory.addLoader(resXMLLoader, 1, "resource");

    GHStateMachineXMLLoader* stateMachineLoader = new GHStateMachineXMLLoader(appShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory(), systemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(stateMachineLoader, 1, "stateMachine");

    GHXMLRedirectLoader* redirectLoader = new GHXMLRedirectLoader(appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(redirectLoader, 1, "redirect");

    GHPropertyStringGeneratorXMLLoader* psg = new GHPropertyStringGeneratorXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(psg, 1, "propStringGenerator");

    GHMessageXMLLoader* messageLoader = new GHMessageXMLLoader(*propsLoader, systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(messageLoader, 1, "message");

    GHXMLObjLoader* messageSender = new GHMessageSenderTransitionXMLLoader(*messageLoader, systemServices.getEventMgr());
    appShard.mXMLObjFactory.addLoader(messageSender, 1, "messageSender");

    GHXMLObjLoader* messageListener = new GHMessageHandlerTransitionXMLLoader(appShard.mEventMgr, systemServices.getPlatformServices().getIdFactory(), appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(messageListener, 1, "messageHandler");

    GHXMLObjLoader* transitionMessageHandler = new GHTransitionMessageHandlerXMLLoader(appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(transitionMessageHandler, 1, "transitionMessageHandler");

    GHSoundTransitionXMLLoader* soundTransLoader = new GHSoundTransitionXMLLoader(appShard.mResourceFactory, systemServices.getSoundDevice(), systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(soundTransLoader, 1, "soundTransition");

    GHSoundProfileTransitionXMLLoader* soundProfileLoader = new GHSoundProfileTransitionXMLLoader(systemServices.getSoundVolumeMgr(), systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(soundProfileLoader, 1, "soundProfileTransition");

    GHPropertyToggleTransitionXMLLoader* pttl = new GHPropertyToggleTransitionXMLLoader(appShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory(), appShard.mProps);
    appShard.mXMLObjFactory.addLoader(pttl, 1, "propertyToggle");

    GHControllerTransitionXMLLoader* conTransLoader = new GHControllerTransitionXMLLoader(appShard.mXMLObjFactory, appShard.mControllerMgr, systemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(conTransLoader, 1, "controllerTransition");

    GHPropertyCopyTransitionXMLLoader* toGlobalPropCopy = new GHPropertyCopyTransitionXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(toGlobalPropCopy, 1, "propCopy");

    GHTransitionSwitchXMLLoader* switchLoader = new GHTransitionSwitchXMLLoader(appShard.mProps, systemServices.getPlatformServices().getIdFactory(), appShard.mXMLObjFactory, appShard.mControllerMgr);
    appShard.mXMLObjFactory.addLoader(switchLoader, 1, "transitionSwitch");

    GHStateChangeTransitionXMLLoader* stateChangeTransLoader = new GHStateChangeTransitionXMLLoader(systemServices.getPlatformServices().getIdFactory(), systemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(stateChangeTransLoader, 1, "stateChangeTransition");

    GHDebugTransitionXMLLoader* debugTransitionLoader = new GHDebugTransitionXMLLoader;
    appShard.mXMLObjFactory.addLoader(debugTransitionLoader, 2, "debugtransition", "debugTransition");

    GHTransitionListTransitionXMLLoader* transitionListTransLoader = new GHTransitionListTransitionXMLLoader(appShard.mXMLObjFactory);
    appShard.mXMLObjFactory.addLoader(transitionListTransLoader, 1, "transitionListTransition");
}