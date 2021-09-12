// Copyright Golden Hammer Software
#include "GHWinSockSocketMgr.h"
#include "GHWinSockSocket.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHWinSock.h"

GHWinSockSocketMgr::GHWinSockSocketMgr(void)
: mWinSockInitialized(false)
{
}

GHWinSockSocketMgr::~GHWinSockSocketMgr(void)
{
	if (mWinSockInitialized)
	{
		GHWinSock::WSACleanup();
	}
}

GHSocket* GHWinSockSocketMgr::createSocket(const char* addr, const char* port, GHConnectionType type) const
{
	if (!mWinSockInitialized)
	{
		// cheating const here for a lazy init.
		GHWinSockSocketMgr* nonConstThis = const_cast<GHWinSockSocketMgr*>(this);
		nonConstThis->initWinSock();
		if (!mWinSockInitialized) {
			return 0;
		}
	}

	return new GHWinSockSocket(addr, port, type);
}

void GHWinSockSocketMgr::initWinSock(void)
{
	GHWinSock::WSADATA wsaData;
	int initRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (initRes != 0) {
		GHDebugMessage::outputString("WSAStartup failed with error: %d\n", initRes);
		return;
	}
	mWinSockInitialized = true;
}
