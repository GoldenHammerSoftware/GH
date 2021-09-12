// Copyright Golden Hammer Software
#include "GHFileC.h"
#include <assert.h>

GHFileC::GHFileC(FILE* file, FileType type, FileMode mode, long maxLength)
:	mFile(file)
,	mFileType(type)
,	mFileMode(mode)
,	mMaxLength(maxLength)
,	mFileBuffer(0)
,	mBufferSize(0)
{
}

GHFileC::~GHFileC(void)
{
	closeFile();
	if (mFileBuffer) {
		delete [] mFileBuffer;
		mBufferSize = 0;
		mFileBuffer = 0;
	}
}

bool GHFileC::closeFile(void)
{
	bool retVal = true;
	if (mFile) {
		if (::fclose(mFile)) {
			// error while closing.
			retVal = false;
		}
		mFile = 0;
	}
	return retVal;
}

bool GHFileC::readIntoBuffer(void)
{
	if (!mFile) return false;
	if (mFileMode == FM_WRITEONLY) return false;
	
	if(mMaxLength < 0) {
		// obtain file size.
		::fseek(mFile, 0, SEEK_END);
		mBufferSize = ::ftell(mFile);
		::rewind(mFile);
	}
	else {
		mBufferSize = mMaxLength;
	}
	
	// allocate memory to contain the whole file.
	mFileBuffer = new char[mBufferSize+1];
	// copy the file into the buffer.
	mBufferSize = ::fread(mFileBuffer, 1, mBufferSize, mFile);
	mFileBuffer[mBufferSize] = '\0';

	return true;
}

bool GHFileC::getFileBuffer(char*& buffer, size_t& bufferSize)
{
	if (!mFileBuffer) return false;
	buffer = mFileBuffer;
	bufferSize = mBufferSize;
	return true;
}

void GHFileC::releaseFileBuffer(void)
{
	// let go of but don't delete the buffer, someone else owns it now.
	mBufferSize = 0;
	mFileBuffer = 0;
}

bool GHFileC::writeBuffer(const void* buffer, size_t bufferSize)
{
	if (!mFile) return false;
	if (mFileMode == FM_READONLY) return false;
	::fwrite(buffer, 1, bufferSize, mFile);
	return true;
}
