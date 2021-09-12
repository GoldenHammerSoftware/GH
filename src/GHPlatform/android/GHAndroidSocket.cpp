// Copyright 2010 Golden Hammer Software
#include "GHAndroidSocket.h"
#include "GHPlatform/GHDebugMessage.h"

#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <stdlib.h>
#include <cerrno>

// some of the following includes may not be needed.  at least one is needed.
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <arpa/inet.h>

GHAndroidSocket::GHAndroidSocket(const char* addr, const char* port, GHConnectionType type)
: mInitialized(false)
, mSocketId(-1)
{
	int portno = ::atoi(port);
	mSocketId = socket(AF_INET, SOCK_STREAM, 0);
	if (mSocketId < 0)
	{
		GHDebugMessage::outputString("Failed to create empty socket");
		return;
	}

	GHDebugMessage::outputString("Looking up host %s", addr);
	struct hostent* server = gethostbyname(addr);
	if (!server)
	{
		GHDebugMessage::outputString("Failed to find server %s", addr);
		return;
	}
	GHDebugMessage::outputString("server->h_length %d", server->h_length);

	struct sockaddr_in serv_addr;
	bzero((char*)&serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(portno);
	bcopy((char*)server->h_addr, (char*)&serv_addr.sin_addr.s_addr, server->h_length);

	int connectResult = connect(mSocketId, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
	if (connectResult < 0)
	{
		GHDebugMessage::outputString("Failed to connect. Error code %s %d", strerror(errno), errno);
		return;
	}


	mInitialized = true;
}

GHAndroidSocket::~GHAndroidSocket(void)
{
	if (mSocketId != -1)
	{
		close(mSocketId);
	}
}

bool GHAndroidSocket::send(const char* data, size_t dataLen)
{
	if (!mInitialized) return false;
	int sizeSent = write(mSocketId, data, dataLen);
	if (sizeSent != dataLen) {
		GHDebugMessage::outputString("Tried to send %s %d, only sent %d", data, dataLen, sizeSent);
		return false;
	}
	GHDebugMessage::outputString("Sent %s %d", data, dataLen);
	return true;
}

bool GHAndroidSocket::read(char* buffer, size_t bufferLen, size_t& sizeRead)
{
	if (!mInitialized) return false;

	bzero(buffer, bufferLen);
	sizeRead = 0;

	while (sizeRead < bufferLen-1)
	{
		int bytesReadThisCall = ::read(mSocketId, buffer + sizeRead, bufferLen - sizeRead);
		if (bytesReadThisCall < 0)
		{
			GHDebugMessage::outputString("Error reading from socket.");
			mInitialized = false;
			return false;
		}
		if (bytesReadThisCall == 0)
		{
			// end of message.
			break;
		}
		sizeRead += bytesReadThisCall;
	}

	/* we originally only read once here
	sizeRead = ::read(mSocketId, buffer, bufferLen);
	if (*reinterpret_cast<int*>(&sizeRead) < 0)
	{
		GHDebugMessage::outputString("Error reading from socket.");
		mInitialized = false;
		return false;
	}
	*/
	GHDebugMessage::outputString("Read %s from socket", buffer);
	return true;
}
