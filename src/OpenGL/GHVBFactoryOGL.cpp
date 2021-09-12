// Copyright 2010 Golden Hammer Software
#include "GHVBFactoryOGL.h"
#include "GHVertexBuffer.h"
#include "GHVBBlitterIndexOGL.h"
#include "GHVertexStreamOGLVBO.h"

GHVertexStream* GHVBFactoryOGL::createVBStream(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage) const
{
    GHVertexStreamOGLVBO* stream = new GHVertexStreamOGLVBO(format, numVerts, usage);
    return stream;
}

GHVBBlitterIndex* GHVBFactoryOGL::createIBStream(unsigned int numIndex, GHVBUsage::Enum usage) const
{
    GHVBBlitterIndexOGL* blitter = new GHVBBlitterIndexOGL(numIndex);
    return blitter;
}


