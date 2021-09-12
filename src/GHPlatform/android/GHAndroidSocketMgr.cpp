// Copyright 2010 Golden Hammer Software
#include "GHAndroidSocketMgr.h"
#include "GHAndroidSocket.h"

GHSocket* GHAndroidSocketMgr::createSocket(const char* addr, const char* port, GHConnectionType type) const
{
	return new GHAndroidSocket(addr, port, type);
}
