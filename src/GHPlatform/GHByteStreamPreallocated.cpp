// Copyright Golden Hammer Software
#include "GHByteStreamPreallocated.h"
#include <string.h>

GHByteStreamPreallocated::GHByteStreamPreallocated(const char* arr, size_t len)
: mArr(arr)
, mSize(len)
, mCurrPos(0)
{
}

bool GHByteStreamPreallocated::readNext(char*& dest, size_t destLen, size_t& sizeRead)
{
	if (mCurrPos >= mSize) 
	{
		// done reading.
		return false;
	}

	sizeRead = destLen;
	if (mCurrPos + destLen >= mSize)
	{
		sizeRead = mSize - mCurrPos;
	}
	::memcpy(dest, &mArr[mCurrPos], sizeRead*sizeof(char));
	mCurrPos += sizeRead;
	return true;
}

void GHByteStreamPreallocated::reset(void)
{
	mCurrPos = 0;
}
