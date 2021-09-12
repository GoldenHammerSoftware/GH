#pragma once

// we're getting namespace conflicts with winsock functions, so moving it all into a namespace.
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
namespace GHWinSock {
	#include <winsock2.h>
	#include <ws2tcpip.h>

	#define GH_INVALID_SOCKET  (GHWinSock::SOCKET)(~0)
}
