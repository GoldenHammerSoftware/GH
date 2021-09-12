// Copyright Golden Hammer Software
#include "GHPlatformServices.h"
#include "GHDebugMessage.h"

#define ENUMHEADER "GHPlatformIdentifiers.h"
#include "GHString/GHEnumInclude.h"

GHPlatformServices::GHPlatformServices(void)
	: mEnumStore(mIdFactory)
{
	GHPlatformIdentifiers::generateIdentifiers(mIdFactory);
}

GHPlatformServices::~GHPlatformServices(void)
{
	GHDebugMessage::shutdown();
}
