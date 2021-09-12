// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocketMgr.h"

class GHWinSockSocketMgr : public GHSocketMgr
{
public:
	GHWinSockSocketMgr(void);
	~GHWinSockSocketMgr(void);

	virtual GHSocket* createSocket(const char* addr, const char* port, GHConnectionType type) const;

private:
	void initWinSock(void);

private:
	bool mWinSockInitialized;
};
