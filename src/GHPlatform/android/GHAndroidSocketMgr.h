// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocketMgr.h"

// the app must include the INTERNET permission in AndroidManifest.xml to use these calls.
class GHAndroidSocketMgr : public GHSocketMgr
{
public:
	virtual GHSocket* createSocket(const char* addr, const char* port, GHConnectionType type) const;
};
