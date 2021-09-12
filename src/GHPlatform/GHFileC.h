// Copyright Golden Hammer Software
#pragma once

#include "GHFile.h"
#include <stdio.h>

// an implementation of file for C FILE structs.
class GHFileC : public GHFile
{
public:
	GHFileC(FILE* file, FileType type=FT_BINARY, FileMode mode=FM_READONLY, long maxLength=-1);
	~GHFileC(void);
	
	// close out the file, we're done.
	bool closeFile(void);
	
	// read an opened file into a buffer.
	bool readIntoBuffer(void);
	// grab ahold of the read buffer.
	bool getFileBuffer(char*& buffer, size_t& bufferSize);
	// release ownership of the buffer (someone else will delete it).
	void releaseFileBuffer(void);
	
	// write out a buffer to an opened write file.
	bool writeBuffer(const void* buffer, size_t bufferSize);
	
	FILE* getFILE(void) { return mFile; }
	
protected:
	// application file for reading.
	FILE* mFile;
	// binary/text.
	FileType mFileType;
	// read mode.
	FileMode mFileMode;
	long mMaxLength;
	
	// a buffer for reading a file into memory.
	// todo: support streaming reading.
	char* mFileBuffer;
	size_t mBufferSize;
};
