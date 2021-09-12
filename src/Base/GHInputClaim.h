// Copyright Golden Hammer Software
#pragma once

// interface for claiming/releasing pointers
class GHInputClaim
{
public:
    virtual ~GHInputClaim(void) {}
    
    virtual bool claimPointer(unsigned int index, void* owner) = 0;
    virtual void releasePointer(unsigned int index, void* owner) = 0;
};
