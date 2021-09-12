// Copyright Golden Hammer Software
#pragma once

#include "GHByteStream.h"

// implementation of byte stream that wraps a pre-allocated array.
class GHByteStreamPreallocated : public GHByteStream
{
public:
	// we do not take ownership of arr, but we expect it to stay alive as long as we do.
	GHByteStreamPreallocated(const char* arr, size_t len);

	virtual bool readNext(char*& dest, size_t destLen, size_t& sizeRead);
	virtual void reset(void);

private:
	// our source buffer.
	const char* mArr;
	// size of the source buffer.
	size_t mSize;
	// current position in arr.
	size_t mCurrPos;
};
