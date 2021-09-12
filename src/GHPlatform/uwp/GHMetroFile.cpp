// Copyright 2010 Golden Hammer Software
#include "GHMetroFile.h"
#include "GHPlatform/GHFileC.h"
#include "GHPlatform/GHDebugMessage.h"

GHMetroFile::GHMetroFile(HANDLE handle, const char* fileName)
	: mFileHandle(handle)
	, mFileBuffer(0)
	, mFileSize(0)
	, mIsValid(true)
	, mIsOpen(true)
{
	mFilename.setConstChars(fileName, GHString::CHT_COPY);
	FILE_STANDARD_INFO fileInfo = {0}; 
	if (mFileHandle.Get() == INVALID_HANDLE_VALUE
		||
		!GetFileInformationByHandleEx( 
			mFileHandle.Get(), 
			FileStandardInfo, 
			&fileInfo, 
			sizeof(fileInfo))) 
	{
		DWORD error = GetLastError();
		if (error != 2) {
			GHDebugMessage::outputString("Failed to open file %s with access error %d", mFilename.getChars(), error);
		}
		mIsValid = false;
		return;
	}

	if (fileInfo.EndOfFile.HighPart != 0) 
    { 
		GHDebugMessage::outputString("Failed to open file %s due to being out of memory.", mFilename.getChars());
		mIsValid = false;
		return;
    } 

	mFileSize = fileInfo.EndOfFile.LowPart;
}

GHMetroFile::~GHMetroFile(void)
{
	closeFile();
	delete [] mFileBuffer;
}

bool GHMetroFile::isValid(void) const
{
	return mIsValid;
}

bool GHMetroFile::closeFile(void)
{
	if (mIsValid && mIsOpen)
	{
		mFileHandle.Close();
		mIsOpen = false;
	}
	return true;
}

bool GHMetroFile::readIntoBuffer(void)
{
	if (!mIsValid || !mIsOpen) return false;
	if (mFileBuffer) return true;

	mFileBuffer = new char[mFileSize+1];
	mFileBuffer[mFileSize] = '\0';
 
    if (!ReadFile( 
		mFileHandle.Get(), 
		mFileBuffer, 
        mFileSize, 
        nullptr, 
        nullptr 
        )) 
    { 
		GHDebugMessage::outputString("Error reading file %s.", mFilename.getChars());
		delete[] mFileBuffer;
		mFileBuffer = 0;
        return false;
    } 

	return true;
}

bool GHMetroFile::getFileBuffer(char*& buffer, size_t& bufferSize)
{
	if (!mFileBuffer)
	{
		return false;
	}

	buffer = mFileBuffer;
	bufferSize = mFileSize;

	return true;
}

void GHMetroFile::releaseFileBuffer(void)
{
	mFileBuffer = 0;
	mFileSize = 0;
}

bool GHMetroFile::writeBuffer(const void* buffer, size_t bufferSize)	
{
	if (!mIsValid || !mIsOpen) return false;

	DWORD numBytesWritten; 
    if (!WriteFile( 
		mFileHandle.Get(), 
        buffer, 
        bufferSize, 
        &numBytesWritten, 
        nullptr)
		|| numBytesWritten != bufferSize) 
    { 
		GHDebugMessage::outputString("Failed to write file %s", mFilename.getChars());
		return false;
    } 
 
    return true; 
}
