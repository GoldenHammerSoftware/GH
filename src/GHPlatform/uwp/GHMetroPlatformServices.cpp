#include "GHMetroPlatformServices.h"

#define ENUMHEADER "GHMetroIdentifiers.h" 
#include "GHString/GHEnumInclude.h"

#include "GHPlatform/GHDebugMessage.h"

GHMetroPlatformServices::GHMetroPlatformServices(void)
	: mFileOpener(L"\\", L"\\")
{
	GHMetroIdentifiers::generateIdentifiers(getIdFactory());

	GHDebugMessage::init(mOutputPipe);
}

GHMetroPlatformServices::~GHMetroPlatformServices(void)
{
}
