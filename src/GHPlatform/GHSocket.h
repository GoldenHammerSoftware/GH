// Copyright Golden Hammer Software
#pragma once
#include "stddef.h"

// interface for a connected socket
class GHSocket
{
public:
	virtual ~GHSocket(void) {}

	// send a string of chars across our network.
	// return false if the sending fails for some reason.
	virtual bool send(const char* data, size_t dataLen) = 0;
	// if there is data to read, read no more than bufferLen chars. 
	// fill in sizeRead with the number of bytes read.
	// returns false if we're disconnected.
	virtual bool read(char* buffer, size_t bufferLen, size_t& sizeRead) = 0;
};
