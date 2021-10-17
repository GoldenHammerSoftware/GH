#pragma once

#include "GHDX12Include.h"
#include "Render/GHVertexStream.h"
#include "GHDX12Buffer.h"

class GHRenderDeviceDX12;
class GHVertexBuffer;

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
    GHDX12Buffer mBuffer;
    D3D12_VERTEX_BUFFER_VIEW mDXView;
};