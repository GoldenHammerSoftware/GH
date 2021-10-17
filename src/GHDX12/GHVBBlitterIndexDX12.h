#pragma once

#include "Render/GHVBBlitter.h"
#include "GHDX12Include.h"
#include <vector>
#include "Render/GHVertexStreamFormat.h"
#include "GHDX12Buffer.h"

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
    void createIED(GHVertexBuffer& vb);
    int applyStreamComponentsToIED(const std::vector<GHVertexStreamFormat::ComponentEntry>& comps, int streamIdx, int localCompIdx);

private:
    GHRenderDeviceDX12& mDevice;
    GHDX12Buffer mBuffer;
    D3D12_INDEX_BUFFER_VIEW mDXView;

    // triangle list, triangle strip, etc.
    // in our case, always triangle list.
    D3D12_PRIMITIVE_TOPOLOGY mD3DPrimitiveType{ D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST };
    // array describing what is contained in the buffer.
    D3D12_INPUT_ELEMENT_DESC* mD3DIED;
    // the number of entries in mD3DIED
    unsigned int mInputElementCount;
};