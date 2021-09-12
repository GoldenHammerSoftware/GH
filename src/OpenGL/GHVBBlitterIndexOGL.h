// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVBBlitter.h"
#include "GHOGLInclude.h"

class GHVBBlitterIndexOGL : public GHVBBlitterIndex
{
public:
    GHVBBlitterIndexOGL(unsigned int numIndices);
    ~GHVBBlitterIndexOGL(void);
    
    virtual void prepareVB(GHVertexBuffer& vb);
    virtual void endVB(GHVertexBuffer& vb);
    virtual void blit(void);

    virtual unsigned short* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const unsigned short* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;
    
private:
    void createVBO(void);
    
private:
    unsigned short* mBuffer;
    
    GLuint mVBO;
    bool mVBOCreated;
    
    // a way to keep track of whether our vbo is up to date.
    unsigned int mVersion;
    unsigned int mVBOVersion;
};
