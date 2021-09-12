// Copyright Golden Hammer Software
#include "GHUtilsServices.h"
#include "GHPlatform/GHPlatformServices.h"
#include "GHProfiler.h"
#include "GHPlatform/GHThreadFactory.h"

#define ENUMHEADER "GHUtilsIdentifiers.h"
#include "GHString/GHEnumInclude.h"

#define ENUMHEADER "GHAlign.h"
#include "GHString/GHEnumInclude.h"

#define ENUMHEADER "GHUtilsMessageTypes.h" 
#include "GHString/GHEnumInclude.h"

GHUtilsServices::GHUtilsServices(GHPlatformServices& platformServices)
{
	GHUtilsIdentifiers::generateIdentifiers(platformServices.getIdFactory());
	GHAlign::generateEnum(platformServices.getEnumStore());
	GHUtilsMessageTypes::generateIdentifiers(platformServices.getIdFactory());

	GHProfiler::initProfiler(platformServices.getTimeCalculator(), 
		platformServices.getThreadFactory().createMutex());

}
