#include "GHNullPlatformServices.h"
#include "GHPlatform/GHDebugMessage.h"

GHNullPlatformServices::GHNullPlatformServices(void)
{
	GHDebugMessage::init(mOutputPipe);
}