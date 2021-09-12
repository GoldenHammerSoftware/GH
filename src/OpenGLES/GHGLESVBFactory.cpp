// Copyright 2010 Golden Hammer Software
#include "GHGLESVBFactory.h"
#include "GHVertexBuffer.h"
#include "GHGLESVBBlitterIndex.h"
#include "GHGLESVertexStreamVBO.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGLESErrorReporter.h"
#include "GHGLESExtensions.h"

GHGLESVBFactory::GHGLESVBFactory(GHEventMgr& eventMgr)
: mEventMgr(eventMgr)
{
    if (GHGLESExtensions::checkAtLeastES3())
    {
        mVAOSupported = true;
    }

    if (mVAOSupported) { GHDebugMessage::outputString("Using vertex array objects for extra speedy goodness"); }
    else { GHDebugMessage::outputString("Not using super fast vertex array objects"); }
}

GHVertexStream* GHGLESVBFactory::createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
{
    GHGLESVertexStreamVBO* stream = new GHGLESVertexStreamVBO(mEventMgr, format, numVerts, usage);
    return stream;
}

GHVBBlitterIndex* GHGLESVBFactory::createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const
{
    GHGLESVBBlitterIndex* blitter = new GHGLESVBBlitterIndex(mEventMgr, numIndex, mVAOSupported);
    return blitter;
}


