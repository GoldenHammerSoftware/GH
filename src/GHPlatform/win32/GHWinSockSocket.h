// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocket.h"
#include "GHPlatform/GHConnectionType.h"
#include "GHWinsock.h"

class GHWinSockSocket : public GHSocket
{
public:
	GHWinSockSocket(const char* addr, const char* port, GHConnectionType type);
	~GHWinSockSocket(void);

	virtual bool send(const char* data, size_t dataLen);
	virtual bool read(char* buffer, size_t bufferLen, size_t& sizeRead);

private:
	GHWinSock::SOCKET mWSSocket;
	bool mConnected;
};