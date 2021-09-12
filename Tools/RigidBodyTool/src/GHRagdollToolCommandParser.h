// Copyright 2010 Golden Hammer Software
#pragma once
#include "GHIdentifierMap.h"

class GHRagdollToolCommandHandler;
class GHPropertyContainer;

class GHRagdollToolCommandParser
{
public:
    GHRagdollToolCommandParser(GHRagdollToolCommandHandler& handler,
                               const GHIdentifierMap<int>& enumStore,
                               GHPropertyContainer& props);
    
    void parseCommand(const char* command);
    
private:
    GHRagdollToolCommandHandler& mHandler;
    const GHIdentifierMap<int>& mEnumStore;
    GHPropertyContainer& mProps;
    
    const static int BUFLEN = 256;
    char mBuffer[BUFLEN];
};
