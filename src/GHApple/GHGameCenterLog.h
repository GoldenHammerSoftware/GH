// Copyright 2010 Golden Hammer Software
#include "GHPlatform/GHDebugMessage.h"

//Does not appear to cause a threading crash,
// but might want to disable just to be safe.
#if 1
#define GHGameCenterLOG GHDebugMessage::outputString
#else
#define GHGameCenterLOG //
#endif

namespace GHGameCenterLog
{
    void printError(NSError* error);
}
