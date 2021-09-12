// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocket.h"
#include "GHPlatform/GHConnectionType.h"

// interface for a connected socket
class GHAndroidSocket : public GHSocket
{
public:
	GHAndroidSocket(const char* addr, const char* port, GHConnectionType type);
	~GHAndroidSocket(void);

	virtual bool send(const char* data, size_t dataLen);
	virtual bool read(char* buffer, size_t bufferLen, size_t& sizeRead);

private:
	bool mInitialized;
	int mSocketId;
};
