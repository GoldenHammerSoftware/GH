// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocketMgr.h"

class GHMetroSocketMgr : public GHSocketMgr
{
public:
	virtual GHSocket* createSocket(const char* addr, const char* port, GHConnectionType type) const;
};
