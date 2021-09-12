// Copyright Golden Hammer Software
#include "GHVertexBuffer.h"
#include <assert.h>
#include "GHRenderStats.h"

GHVertexBuffer::GHVertexBuffer(void)
: mBlitter(0)
{
}

GHVertexBuffer::~GHVertexBuffer(void)
{
    if (mBlitter) mBlitter->release();
    std::vector<GHVertexStreamPtr*>::iterator streamIter;
    for (streamIter = mStreams.begin(); streamIter != mStreams.end(); ++streamIter)
    {
        (*streamIter)->release();
    }
}

void GHVertexBuffer::prepareVB(void)
{
    if (mBlitter) mBlitter->get()->prepareVB(*this);
}

void GHVertexBuffer::endVB(void)
{
    if (mBlitter) mBlitter->get()->endVB(*this);
}

void GHVertexBuffer::blit(void)
{
	GHRenderStats::recordDraw(mStreams[0]->get()->getNumVerts());
    if (mBlitter) mBlitter->get()->blit();
}

void GHVertexBuffer::replaceStream(unsigned int streamId, GHVertexStreamPtr* stream)
{
    if (mStreams.size() <= streamId) mStreams.resize(streamId+1, 0);
    stream->acquire();
    if (mStreams[streamId]) mStreams[streamId]->release();
    mStreams[streamId] = stream;
}

GHVertexStreamPtr* GHVertexBuffer::getStream(unsigned int streamId)
{
    assert(streamId < mStreams.size());
    return mStreams[streamId];
}

void GHVertexBuffer::replaceBlitter(GHVBBlitterPtr* blitter)
{
    blitter->acquire();
    if (mBlitter) mBlitter->release();
    mBlitter = blitter;
}

GHVertexBuffer* GHVertexBuffer::clone(void)
{
    GHVertexBuffer* ret = new GHVertexBuffer();
	for (size_t i = 0; i < mStreams.size(); ++i)
    {
        if (mStreams[i]->get()->getFormat().isShared())
        {
            ret->replaceStream((unsigned int)i, mStreams[i]);
        }
        else
        {
            ret->replaceStream((unsigned int)i, new GHVertexStreamPtr(mStreams[i]->get()->clone()));
        }
    }
    // todo? support non-shared blitters.
    ret->replaceBlitter(mBlitter);
    return ret;
}


