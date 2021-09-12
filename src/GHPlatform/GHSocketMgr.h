// Copyright Golden Hammer Software
#pragma once

#include "GHConnectionType.h"

class GHSocket;

// interface for creating network connections.
class GHSocketMgr
{
public:
	virtual ~GHSocketMgr(void) {}

	// create a connected socket if possible, otherwise return 0.
	// caller is expected to delete.
	virtual GHSocket* createSocket(const char* addr, const char* port, GHConnectionType type) const = 0;
};
