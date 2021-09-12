// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocketMgr.h"

class GHNullSocketMgr : public GHSocketMgr
{
public:
	virtual GHSocket* createSocket(const char* addr, const char* port, GHConnectionType type) const { return 0; }
};
