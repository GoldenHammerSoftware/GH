// Copyright Golden Hammer Software
#include "GHPropertyXMLInitializer.h"
#include "GHAppShard.h"
#include "GHSystemServices.h"
#include "GHPropertyFloatXMLLoader.h"
#include "GHPropertyIntXMLLoader.h"
#include "GHPropertyEnumXMLLoader.h"
#include "GHPropertyBoolXMLLoader.h"
#include "GHPropertyPointXMLLoader.h"
#include "GHPropertyIdentifierXMLLoader.h"
#include "GHPropertyStringXMLLoader.h"
#include "GHPropertyContainerXMLLoader.h"
#include "GHPropertyCopyXMLLoader.h"
#include "GHPropertyTypeXMLLoader.h"
#include "GHUtils/GHTransition.h"

void GHPropertyXMLInitializer::initAppShard(GHAppShard& appShard, GHSystemServices& systemServices)
{
    GHPropertyContainerXMLLoader* propsLoader = new GHPropertyContainerXMLLoader(appShard.mXMLObjFactory, systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(propsLoader, 1, "properties");
    
    GHPropertyFloatXMLLoader* propFloatLoader = new GHPropertyFloatXMLLoader;
    appShard.mXMLObjFactory.addLoader(propFloatLoader, 1, "floatProp");
    
    GHPropertyIntXMLLoader* propIntLoader = new GHPropertyIntXMLLoader;
    appShard.mXMLObjFactory.addLoader(propIntLoader, 1, "intProp");
    
    GHPropertyEnumXMLLoader* enumLoader = new GHPropertyEnumXMLLoader(systemServices.getPlatformServices().getEnumStore());
    appShard.mXMLObjFactory.addLoader(enumLoader, 1, "enumProp");
    
    GHPropertyBoolXMLLoader* boolLoader = new GHPropertyBoolXMLLoader();
    appShard.mXMLObjFactory.addLoader(boolLoader, 1, "boolProp");
    
	GHPropertyCopyXMLLoader* copyLoader = new GHPropertyCopyXMLLoader(systemServices.getPlatformServices().getIdFactory());
	appShard.mXMLObjFactory.addLoader(copyLoader, 1, "copyProp");

    GHPropertyPointXMLLoader<2>* p2Loader = new GHPropertyPointXMLLoader<2>;
    appShard.mXMLObjFactory.addLoader(p2Loader, 1, "point2Prop");
    GHPropertyPointXMLLoader<3>* p3Loader = new GHPropertyPointXMLLoader<3>;
    appShard.mXMLObjFactory.addLoader(p3Loader, 1, "point3Prop");
    GHPropertyPointXMLLoader<4>* p4Loader = new GHPropertyPointXMLLoader<4>;
    appShard.mXMLObjFactory.addLoader(p4Loader, 1, "point4Prop");
    GHPropertyPointXMLLoader<16>* p16Loader = new GHPropertyPointXMLLoader<16>;
    appShard.mXMLObjFactory.addLoader(p16Loader, 1, "point16Prop");
    
    GHPropertyIdentifierXMLLoader* idPropLoader = new GHPropertyIdentifierXMLLoader(systemServices.getPlatformServices().getIdFactory());
    appShard.mXMLObjFactory.addLoader(idPropLoader, 1, "identifierProp");
    
    GHPropertyStringXMLLoader* stringPropLoader = new GHPropertyStringXMLLoader();
    appShard.mXMLObjFactory.addLoader(stringPropLoader, 1, "stringProp");

	GHPropertyTypeXMLLoader<GHTransition, false>* transitionPropLoader = new GHPropertyTypeXMLLoader < GHTransition, false > (appShard.mXMLObjFactory);
	appShard.mXMLObjFactory.addLoader(transitionPropLoader, 1, "transitionProp");
}
