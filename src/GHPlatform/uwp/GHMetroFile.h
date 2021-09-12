// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHPlatform/GHFile.h"
#include <wrl.h>
#include "GHString/GHString.h"

class GHMetroFile : public GHFile
{
public:
	GHMetroFile(HANDLE handle, const char* fileName);
	~GHMetroFile(void);

public:
	bool isValid(void) const;

public:

	virtual bool closeFile(void);
	
	// read an opened file into a buffer.
	virtual bool readIntoBuffer(void);
	// grab ahold of the read buffer.
	virtual bool getFileBuffer(char*& buffer, size_t& bufferSize);
	// release ownership of the buffer (someone else will delete it).
	virtual void releaseFileBuffer(void);
	
	// write out a buffer to an opened write file.
	virtual bool writeBuffer(const void* buffer, size_t bufferSize);

private:
	GHString mFilename;
	Microsoft::WRL::Wrappers::FileHandle mFileHandle;
	bool mIsValid;
	bool mIsOpen;
	char* mFileBuffer;
	DWORD mFileSize;
};
