// Copyright Golden Hammer Software
#pragma once

#include <stdlib.h>

// class to wrap raw file IO.
class GHFile
{
public:
	// enum for file formats.
	typedef enum {
		FT_BINARY,
		FT_TEXT,
	} FileType;
	
	// enum for file modes.
	typedef enum {
		FM_READONLY,
		FM_WRITEONLY,
		FM_READWRITE,
	} FileMode;
	
public:
	virtual ~GHFile(void) {}
	
	// close out the file, we're done.
	virtual bool closeFile(void) = 0;
	
	// read an opened file into a buffer.
	virtual bool readIntoBuffer(void) = 0;
	// grab ahold of the read buffer.
	virtual bool getFileBuffer(char*& buffer, size_t& bufferSize) = 0;
	// release ownership of the buffer (someone else will delete it).
	virtual void releaseFileBuffer(void) = 0;
	
	// write out a buffer to an opened write file.
	virtual bool writeBuffer(const void* buffer, size_t bufferSize) = 0;
};
