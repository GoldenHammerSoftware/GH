// Copyright 2010 Golden Hammer Software
#include "GHCFSocketMgr.h"
#include "GHCFSocket.h"
#include "GHDebugMessage.h"

#import "UIKit/UIKit.h"
#import <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>

GHSocket* GHCFSocketMgr::createSocket(const char* addrChr, const char* port, GHConnectionType type) const
{
    // todo: udp
    assert(type == CT_TCP);
    
    if (!addrChr) return 0;
    
    // CFNetwork stuff is iOS 5.0+ and OSX 10.7
    // todo: wrap this version check in an ios ifdef.
    NSString *version = [[UIDevice currentDevice] systemVersion];
    if ([version floatValue] < 5)
    {
        return 0;
    }
    
    int err;
    struct addrinfo addrInfo;
    struct addrinfo* addrResult;
    NSData* address = nil;
    memset(&addrInfo, 0, sizeof(addrInfo));

    // this call will synchronously hit the network DNS resolver.
    err = getaddrinfo(addrChr, port, &addrInfo, &addrResult);
    if (err == 0)
    {
        address = [NSData dataWithBytes:addrResult->ai_addr length:addrResult->ai_addrlen];
        freeaddrinfo(addrResult);
    }
    else
    {
        GHDebugMessage::outputString("getaddrinfo failed with code %d", err);
        return 0;
    }
    
    CFHostRef host = CFHostCreateWithAddress(NULL, (__bridge CFDataRef)address);
    
    CFReadStreamRef readStream = nil;
    CFWriteStreamRef writeStream = nil;
    SInt32 portInt = 0; // todo?
    CFStreamCreatePairWithSocketToCFHost(NULL, host, portInt, &readStream, &writeStream);
    if (!readStream || !writeStream)
    {
        GHDebugMessage::outputString("Failed to create read/write stream for socket");
        return 0;
    }
    Boolean readOpened = CFReadStreamOpen(readStream);
    if (readOpened == FALSE)
    {
        GHDebugMessage::outputString("Failed to open read stream for socket");
        return 0;
    }
    Boolean writeOpened = CFWriteStreamOpen(writeStream);
    if (writeOpened == FALSE)
    {
        GHDebugMessage::outputString("Failed to open write stream for socket");
        return 0;
    }
    
    return new GHCFSocket(readStream, writeStream);
}
