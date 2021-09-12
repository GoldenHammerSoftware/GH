// Copyright Golden Hammer Software
#include "GHXorFile.h"

GHXorFile::GHXorFile(GHFile& src, unsigned char key)
: mSrc(src)
, mKey(key)
, mFileBuffer(0)
, mBufferSize(0)
{
}

GHXorFile::~GHXorFile(void)
{
    if (mFileBuffer) delete mFileBuffer;
}

bool GHXorFile::closeFile(void)
{
    return mSrc.closeFile();
}

bool GHXorFile::readIntoBuffer(void)
{
    return mSrc.readIntoBuffer();
}

bool GHXorFile::getFileBuffer(char*& buffer, size_t& bufferSize)
{
    if (mFileBuffer) {
        buffer = mFileBuffer;
        bufferSize = mBufferSize;
        return true;
    }
    if (!mSrc.getFileBuffer(mFileBuffer, mBufferSize)) {
        return false;
    }
    // we are about to decrypt the file, so detach it from the src.
    mSrc.releaseFileBuffer();
    for (size_t i = 0; i < mBufferSize; ++i) {
        mFileBuffer[i] ^= mKey;
    }
    buffer = mFileBuffer;
    bufferSize = mBufferSize;
    return true;
}

void GHXorFile::releaseFileBuffer(void)
{
    // let go of but don't delete the buffer, someone else owns it now.
	mBufferSize = 0;
	mFileBuffer = 0;
}

bool GHXorFile::writeBuffer(const void* buffer, size_t bufferSize)
{
    bool ret = true;
    const char* charBuff = (const char*)buffer;
    for (size_t i = 0; i < bufferSize; ++i)
    {
        char outChar = (*charBuff);
        outChar ^= mKey;
        ret |= mSrc.writeBuffer(&outChar, 1);
        if (!ret) return false;
        charBuff++;
    }
    return ret;
}
