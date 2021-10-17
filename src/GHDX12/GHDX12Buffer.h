#pragma once

#include "GHDX12Include.h"
#include "Render/GHVBUsage.h"

class GHRenderDeviceDX12;

class GHDX12Buffer
{
public:
    GHDX12Buffer(GHRenderDeviceDX12& device, size_t bufferSize, GHVBUsage::Enum usage);
    ~GHDX12Buffer(void);

    void* lockWriteBuffer(void);
    void unlockWriteBuffer(void);

    const void* lockReadBuffer(void) const;
    void unlockReadBuffer(void) const;

    ID3D12Resource* getDXBuffer(void) const { return mDXBuffer; }
    GHVBUsage::Enum getUsage(void) const { return mUsage; }
    size_t getBufferSize(void) const { return mBufferSize; }

private:
    void initStatic(void);
    void initDynamic(void);

private:
    GHRenderDeviceDX12& mDevice;
    size_t mBufferSize;
    GHVBUsage::Enum mUsage;

    ID3D12Resource* mDXBuffer{ nullptr };
    // cpu only memory copy
    void* mMemoryBuffer{ nullptr };
    // optional upload buffer for static buffers.
    // probably don't need to keep this around.
    ID3D12Resource* mDXUploadBuffer{ nullptr };
};
