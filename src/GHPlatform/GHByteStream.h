// Copyright Golden Hammer Software
#pragma once

#include <cstddef>

// interface for something that can stream out a list of bytes
class GHByteStream
{
public:
	virtual ~GHByteStream(void) {}

	// copy the next chunk of destLen into dest if available.  
	// fill in amount read into sizeRead.
	// if nothing left to read return false.
	virtual bool readNext(char*& dest, size_t destLen, size_t& sizeRead) = 0;
	// reset to the start of the stream if applicable.
	virtual void reset(void) = 0;
};
