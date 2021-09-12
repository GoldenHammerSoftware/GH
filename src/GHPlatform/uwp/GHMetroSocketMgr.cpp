// Copyright 2010 Golden Hammer Software
#include "GHMetroSocketMgr.h"
#include "GHMetroSocketTCP.h"
#include "GHPlatform/GHDebugMessage.h"

GHSocket* GHMetroSocketMgr::createSocket(const char* addr, const char* port, GHConnectionType type) const
{
	if (type == GHConnectionType::CT_UDP)
	{
		GHDebugMessage::outputString("UDP connections not implemented for Win8 yet");
		return 0;
	}
	GHMetroSocketTCP* ret = new GHMetroSocketTCP(addr, port);
	if (!ret->isConnected())
	{
		delete ret;
		ret = 0;
	}
	return ret;
}
