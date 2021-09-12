// Copyright Golden Hammer Software
#pragma once

#include "GHFile.h"

// a file wrapper which encrypts/decrypts read/writes with an xor value.
class GHXorFile : public GHFile
{
public:
    GHXorFile(GHFile& src, unsigned char key);
    ~GHXorFile(void);
    
	virtual bool closeFile(void);
	
	virtual bool readIntoBuffer(void);
	virtual bool getFileBuffer(char*& buffer, size_t& bufferSize);
	virtual void releaseFileBuffer(void);
	virtual bool writeBuffer(const void* buffer, size_t bufferSize);
    
private:
    GHFile& mSrc;
    unsigned char mKey;
    
    // decrypted buffer from source.
    char* mFileBuffer;
	size_t mBufferSize;
};
