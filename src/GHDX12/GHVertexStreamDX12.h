#pragma once

#include "GHDX12Include.h"
#include "Render/GHVertexStream.h"

class GHRenderDeviceDX12;

class GHVertexStreamDX12 : public GHVertexStream
{
public:
    GHVertexStreamDX12(GHRenderDeviceDX12& device, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage);
    ~GHVertexStreamDX12(void);

    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride) override;
    virtual void endVB(int streamIdx) override;

    virtual float* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;

    virtual const float* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;

    virtual GHVertexStream* clone(void) const override;

private:
    GHRenderDeviceDX12& mDevice;
    GHVBUsage::Enum mUsage;
    // we keep a memory copy of the verts for lock read and for partial writes.
    void* mMemoryBuffer;

    ID3D12Resource* mDXBuffer;
    D3D12_VERTEX_BUFFER_VIEW mDXView;
    // probably don't need to keep the upload buffer around.
    ID3D12Resource* mDXUploadBuffer;
};