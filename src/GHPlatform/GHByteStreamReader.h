// Copyright Golden Hammer Software
#pragma once

#include <cstddef>

class GHByteStream;

// something that reads from a GHByteStream and returns individual entries.
// we ask for pieces of the main buffer in chunks
// this class largely exists to provide non-virtual accessors to byte stream reading.
class GHByteStreamReader
{
public:
	// created with a stream source, and a size of how big of a chunk we cache from the stream.
	GHByteStreamReader(GHByteStream& stream, size_t chunkSize);
	~GHByteStreamReader(void);

	char getNextByte(void);
	short getNextShort(void);
	float getNextFloat(void);
	int getNextInt(void);

private:
	GHByteStream& mStream;

	// scratch buffer, we read chunks from the stream into this.
	char* mScratch;
	// size of our scratch buffer;
	size_t mScratchSize;
	// number of bytes currently in our scratch buffer;
	size_t mScratchRead;
	// current position in mScratch for next byte.
	size_t mPositionInScratch;
};
