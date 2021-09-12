// Copyright 2010 Golden Hammer Software
#include "GHCFSocket.h"

GHCFSocket::GHCFSocket(CFReadStreamRef readStream, CFWriteStreamRef writeStream)
: mReadStream(readStream)
, mWriteStream(writeStream)
, mConnected(true)
{
}

GHCFSocket::~GHCFSocket(void)
{
    CFReadStreamClose(mReadStream);
    CFWriteStreamClose(mWriteStream);
}

bool GHCFSocket::send(const char* data, size_t dataLen)
{
    if (!mConnected) return false;
    CFIndex bytesWritten = CFWriteStreamWrite(mWriteStream, (UInt8*)data, dataLen);
    if (bytesWritten < 0) {
        return false;
    }
    return true;
}

bool GHCFSocket::read(char* buffer, size_t bufferLen, size_t& sizeRead)
{
    if (!mConnected) return false;
    
    sizeRead = 0;
    char* readDest = buffer;
    while (sizeRead < bufferLen && CFReadStreamHasBytesAvailable(mReadStream))
    {
        size_t lastSizeRead = CFReadStreamRead(mReadStream, (UInt8*)readDest, bufferLen);
        if (lastSizeRead < 0)
        {
            // socket closed.
            return false;
        }
        sizeRead += lastSizeRead;
        readDest += lastSizeRead;
    }
    return true;
}
