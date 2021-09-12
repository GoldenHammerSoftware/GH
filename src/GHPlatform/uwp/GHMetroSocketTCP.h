// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHSocket.h"

class GHMetroSocketTCP : public GHSocket
{
public:
	GHMetroSocketTCP(const char* addr, const char* port);
	~GHMetroSocketTCP(void);

	virtual bool send(const char* data, size_t dataLen);
	virtual bool read(char* buffer, size_t bufferLen, size_t& sizeRead);

	bool isConnected(void) const { return mConnected; }

private:
	Windows::Networking::Sockets::StreamSocket^ mStreamSocket;
	Windows::Storage::Streams::DataWriter^ mDataWriter;
	Windows::Storage::Streams::DataReader^ mDataReader;
	bool mConnected;
};
