#pragma once

#include "Render/GHVBBlitter.h"
#include "GHDX12Include.h"

class GHRenderDeviceDX12;

class GHVBBlitterIndexDX12 : public GHVBBlitterIndex
{
public:
    GHVBBlitterIndexDX12(GHRenderDeviceDX12& device, unsigned int numIndices);
    ~GHVBBlitterIndexDX12(void);

    virtual void prepareVB(GHVertexBuffer& vb) override;
    virtual void endVB(GHVertexBuffer& vb) override;
    virtual void blit(void) override;

    virtual unsigned short* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;

    virtual const unsigned short* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;

private:
    GHRenderDeviceDX12& mDevice;
    void* mMemoryBuffer{ 0 };

    ID3D12Resource* mDXBuffer;
    D3D12_INDEX_BUFFER_VIEW mDXView;
    // probably don't need to keep the upload buffer around.
    ID3D12Resource* mDXUploadBuffer;
};