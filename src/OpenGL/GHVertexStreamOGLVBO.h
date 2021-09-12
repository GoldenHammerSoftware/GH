// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVertexStream.h"
#include "GHOGLInclude.h"

class GHVertexStreamOGLVBO : public GHVertexStream
{
public:
    GHVertexStreamOGLVBO(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage);
    ~GHVertexStreamOGLVBO(void);
    
    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride);
    virtual void endVB(int streamIdx);
    
    virtual float* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const float* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;

    virtual GHVertexStream* clone(void) const;

private:
    void createVBO(void);
    
private:
    float* mBuffer;
    GHVBUsage::Enum mUsage;
    
    GLuint mVBO;
    bool mVBOCreated;
    
    // a way to keep track of whether our vbo is up to date.
    unsigned int mVersion;
    unsigned int mVBOVersion;
};
