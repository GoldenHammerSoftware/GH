// Copyright Golden Hammer Software
#include "GHVertexStreamOverride.h"

GHVertexStreamOverride::GHVertexStreamOverride(GHVertexStreamFormatPtr* format)
: GHVertexStream(format)
, mSourceStream(0)
{
}

GHVertexStreamOverride::~GHVertexStreamOverride(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mSourceStream, 0);
}

void GHVertexStreamOverride::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
    if (!mSourceStream) return;
    mSourceStream->get()->prepareVB(streamIdx, mFormat->get());
}

void GHVertexStreamOverride::endVB(int streamIdx)
{
    if (!mSourceStream) return;
    mSourceStream->get()->endVB(streamIdx);
}

float* GHVertexStreamOverride::lockWriteBuffer(void)
{
    if (!mSourceStream) return 0;
    return mSourceStream->get()->lockWriteBuffer();
}

void GHVertexStreamOverride::unlockWriteBuffer(void)
{
    if (!mSourceStream) return;
    mSourceStream->get()->unlockWriteBuffer();
}

const float* GHVertexStreamOverride::lockReadBuffer(void) const
{
    if (!mSourceStream) return 0;
    return mSourceStream->get()->lockReadBuffer();
}

void GHVertexStreamOverride::unlockReadBuffer(void) const
{
    if (!mSourceStream) return;
    return mSourceStream->get()->unlockReadBuffer();
}

GHVertexStream* GHVertexStreamOverride::clone(void) const
{
    GHVertexStreamOverride* ret = new GHVertexStreamOverride(mFormat);
    ret->setSourceStream(mSourceStream);
    return ret;
}

void GHVertexStreamOverride::setSourceStream(GHVertexStreamPtr* sourceStream)
{
    GHRefCounted::changePointer((GHRefCounted*&)mSourceStream, sourceStream);
}
