// Copyright Golden Hammer Software
#include "GHWinSockSocket.h"
#include "GHPlatform/GHDebugMessage.h"
#include <stdio.h>

static const char* convertGetAddrInfoErrorToString(int result)
{
	if (result == WSATRY_AGAIN) return "WSATRY_AGAIN";
	if (result == WSAEINVAL) return "WSAEINVAL";
	if (result == WSANO_RECOVERY) return "WSANO_RECOVERY";
	if (result == WSAEAFNOSUPPORT) return "WSAEAFNOSUPPORT";
	if (result == WSA_NOT_ENOUGH_MEMORY) return "WSA_NOT_ENOUGH_MEMORY";
	if (result == WSAHOST_NOT_FOUND) return "WSAHOST_NOT_FOUND";
	if (result == WSATYPE_NOT_FOUND) return "WSATYPE_NOT_FOUND";
	if (result == WSAESOCKTNOSUPPORT) return "WSAESOCKTNOSUPPORT";
	return "UNKNOWN_ERROR";
}

GHWinSockSocket::GHWinSockSocket(const char* addr, const char* port, GHConnectionType type)
: mWSSocket(GH_INVALID_SOCKET)
, mConnected(false)
{
	// convert the addr into something we can use.
	GHWinSock::addrinfo lookupHints;
	ZeroMemory(&lookupHints, sizeof(lookupHints));
	lookupHints.ai_family = AF_UNSPEC;
	lookupHints.ai_socktype = SOCK_STREAM;
	if (type == GHConnectionType::CT_TCP) {
		lookupHints.ai_protocol = GHWinSock::IPPROTO_TCP;
	}
	else {
		lookupHints.ai_protocol = GHWinSock::IPPROTO_UDP;
	}
	GHWinSock::addrinfo* lookupDest;
	int lookupResult = getaddrinfo(addr, port, &lookupHints, &lookupDest);
	if (lookupResult != 0) {
		GHDebugMessage::outputString("getaddrinfo failed with error: %s\n", convertGetAddrInfoErrorToString(lookupResult));
		mConnected = false;
		return;
	}

	// Attempt to connect to an address until one succeeds
	for (GHWinSock::addrinfo* ptr = lookupDest; ptr != NULL; ptr = ptr->ai_next) {

		// Create a SOCKET for connecting to server
		mWSSocket = GHWinSock::socket(ptr->ai_family, ptr->ai_socktype,
			ptr->ai_protocol);
		if (mWSSocket == GH_INVALID_SOCKET) {
			GHDebugMessage::outputString("socket failed with error: %ld\n", GHWinSock::WSAGetLastError());
			mConnected = false;
			freeaddrinfo(lookupDest);
			return;
		}

		// Connect to server.
		int iResult = connect(mWSSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
		if (iResult == SOCKET_ERROR) {
			GHWinSock::closesocket(mWSSocket);
			mWSSocket = GH_INVALID_SOCKET;
			continue;
		}
		break;
	}

	if (mWSSocket == GH_INVALID_SOCKET)
	{
		GHDebugMessage::outputString("Failed to open socket");
		freeaddrinfo(lookupDest);
		mConnected = false;
		return;
	}

	mConnected = true;
	freeaddrinfo(lookupDest);
}

GHWinSockSocket::~GHWinSockSocket(void)
{
	if (mWSSocket != GH_INVALID_SOCKET)
	{
		GHWinSock::closesocket(mWSSocket);
	}
}

bool GHWinSockSocket::send(const char* data, size_t dataLen)
{
	if (!mConnected) return false;

	int iResult = GHWinSock::send(mWSSocket, data, (int)strlen(data), 0);
	if (iResult == SOCKET_ERROR) {
		printf("send failed with error: %d\n", GHWinSock::WSAGetLastError());
		mConnected = false;
		return false;
	}
	return true;
}

bool GHWinSockSocket::read(char* buffer, size_t bufferLen, size_t& sizeRead)
{
	if (!mConnected) return false;

	int iResult = GHWinSock::recv(mWSSocket, buffer, (int)bufferLen, 0);
	if (iResult > 0) {
		GHDebugMessage::outputString("Bytes received: %d\n", iResult);
	}
	else if (iResult == 0) {
		GHDebugMessage::outputString("Connection closed\n");
		mConnected = false;
	}
	else {
		GHDebugMessage::outputString("recv failed with error: %d\n", GHWinSock::WSAGetLastError());
	}

//	GHDebugMessage::outputString(buffer);
	sizeRead = iResult;
	return mConnected;
}
