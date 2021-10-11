#pragma once

#include "Render/GHVertexStream.h"

class GHVertexStreamDX12 : public GHVertexStream
{
public:
    GHVertexStreamDX12(GHVertexStreamFormatPtr* format);
    ~GHVertexStreamDX12(void);

    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride) override;
    virtual void endVB(int streamIdx) override;

    virtual float* lockWriteBuffer(void) override;
    virtual void unlockWriteBuffer(void) override;

    virtual const float* lockReadBuffer(void) const override;
    virtual void unlockReadBuffer(void) const override;

    virtual GHVertexStream* clone(void) const override;
};