#include "GHWin32PlatformServices.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHThreadFactory.h"

GHWin32PlatformServices::GHWin32PlatformServices(HWND hwnd)
	: mFilePicker(hwnd)
{
	GHDebugMessage::init(mOutputPipe);
}

GHWin32PlatformServices::~GHWin32PlatformServices(void)
{

}
