// Copyright Golden Hammer Software
#include "GHByteStreamReader.h"
#include "GHByteStream.h"
#include <string.h>
#include "GHDebugMessage.h"

GHByteStreamReader::GHByteStreamReader(GHByteStream& stream, size_t chunkSize)
: mStream(stream)
, mScratchSize(chunkSize)
, mScratchRead(0)
, mPositionInScratch(0)
{
	mScratch = new char[mScratchSize];
	memset(mScratch, 0, mScratchSize*sizeof(char));
}

GHByteStreamReader::~GHByteStreamReader(void)
{
	delete [] mScratch;
}

char GHByteStreamReader::getNextByte(void)
{
	if (!mScratchRead || 
		mPositionInScratch >= mScratchRead)
	{
		bool success = mStream.readNext(mScratch, mScratchSize, mScratchRead);
		mPositionInScratch = 0;
		if (!success)
		{
			return 0;
		}
	}
	char ret = mScratch[mPositionInScratch];
	mPositionInScratch++;
	return ret;
}

short GHByteStreamReader::getNextShort(void)
{
	union ShortUnion
	{
		char mChars[2];
		short mShort;
	};
	ShortUnion shortUnion;
	shortUnion.mChars[0] = getNextByte();
	shortUnion.mChars[1] = getNextByte();
	return shortUnion.mShort;
}

float GHByteStreamReader::getNextFloat(void)
{
	union FloatUnion
	{
		char mChars[4];
		float mFloat;
	};
	FloatUnion un;
	un.mChars[0] = getNextByte();
	un.mChars[1] = getNextByte();
	un.mChars[2] = getNextByte();
	un.mChars[3] = getNextByte();
	return un.mFloat;
}

int GHByteStreamReader::getNextInt(void)
{
	union IntUnion
	{
		char mChars[4];
		int mInt;
	};
	IntUnion un;
	un.mChars[0] = getNextByte();
	un.mChars[1] = getNextByte();
	un.mChars[2] = getNextByte();
	un.mChars[3] = getNextByte();
	return un.mInt;
}
