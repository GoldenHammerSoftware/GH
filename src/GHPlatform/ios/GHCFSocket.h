// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHSocket.h"

// CFNetwork implementation of socket
class GHCFSocket : public GHSocket
{
public:
    GHCFSocket(CFReadStreamRef readStream, CFWriteStreamRef writeStream);
    ~GHCFSocket(void);
    
    virtual bool send(const char* data, size_t dataLen);
    virtual bool read(char* buffer, size_t bufferLen, size_t& sizeRead);
    
private:
    CFReadStreamRef mReadStream;
    CFWriteStreamRef mWriteStream;
    // flag we set on error.
    bool mConnected;
};
