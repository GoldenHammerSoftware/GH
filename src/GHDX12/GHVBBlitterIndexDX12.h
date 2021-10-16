#pragma once

#include "Render/GHVBBlitter.h"

class GHVBBlitterIndexDX12 : public GHVBBlitterIndex
{
public:
    GHVBBlitterIndexDX12(unsigned int numIndices);
    ~GHVBBlitterIndexDX12(void);

    virtual void prepareVB(GHVertexBuffer& vb) override;
    virtual void endVB(GHVertexBuffer& vb) override;
    virtual void blit(void) override;

    virtual unsigned short* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;

    virtual const unsigned short* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;

private:
    void* mMemoryBuffer{ 0 };
};