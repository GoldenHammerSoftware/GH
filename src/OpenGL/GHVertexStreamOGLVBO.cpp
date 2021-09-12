// Copyright 2010 Golden Hammer Software
#include "GHVertexStreamOGLVBO.h"
#include "GHGLErrorReporter.h"

GHVertexStreamOGLVBO::GHVertexStreamOGLVBO(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage)
: GHVertexStream(format)
, mUsage(usage)
, mVersion(1)
, mVBOVersion(0)
, mVBOCreated(false)
{
    mNumVerts = numVerts;
    mBuffer = new float[mNumVerts * format->get()->getVertexSize()];
}

GHVertexStreamOGLVBO::~GHVertexStreamOGLVBO(void)
{
    delete [] mBuffer;
    if (mVBOCreated)
    {
        glDeleteBuffers(1, &mVBO);
    }
}

void GHVertexStreamOGLVBO::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
    GHVertexStreamFormat* format = formatOverride ? formatOverride : mFormat->get();
    if (mVersion != mVBOVersion)
    {
        createVBO();
    }
	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    
    unsigned short vertSize = format->getVertexSize();
    const std::vector<GHVertexStreamFormat::ComponentEntry>& components = format->getComponents();
    for (int i = 0; i < components.size(); ++i)
    {
        const GHVertexStreamFormat::ComponentEntry& comp = components[i];
        
		glEnableVertexAttribArray(comp.mComponent);
        
        int glType = GL_BYTE;
        if (comp.mType == GHVertexComponentType::CT_BYTE)
        {
        }
        else if (comp.mType == GHVertexComponentType::CT_UBYTE)
        {
            glType = GL_UNSIGNED_BYTE;
        }
        else if (comp.mType == GHVertexComponentType::CT_SHORT)
        {
            glType = GL_SHORT;
        }
        else if (comp.mType == GHVertexComponentType::CT_FLOAT)
        {
            glType = GL_FLOAT;
        }
        glVertexAttribPointer(comp.mComponent, comp.mCount, glType, false, 
                              vertSize*sizeof(float),
                              (GLvoid*)(comp.mOffset*sizeof(float)));
    }
}

void GHVertexStreamOGLVBO::endVB(int streamIdx)
{
	glBindBuffer(GL_ARRAY_BUFFER, 0);

    // todo?: don't disable unless the next buffer isn't going to use the types.
    const std::vector<GHVertexStreamFormat::ComponentEntry>& components = mFormat->get()->getComponents();
    for (int i = 0; i < components.size(); ++i)
    {
		glDisableVertexAttribArray(components[i].mComponent);
    }
}

float* GHVertexStreamOGLVBO::lockWriteBuffer(void)
{
    return mBuffer;
}

void GHVertexStreamOGLVBO::unlockWriteBuffer(void)
{
    mVersion++;
}

const float* GHVertexStreamOGLVBO::lockReadBuffer(void) const
{
    return mBuffer;
}

void GHVertexStreamOGLVBO::unlockReadBuffer(void) const
{
    
}

void GHVertexStreamOGLVBO::createVBO(void)
{
    if (!mVBOCreated)
    {
        glGenBuffers(1, &mVBO);
        GHGLErrorReporter::checkAndReportError("glGenBuffers - GHVertexStreamOGLVBO");
        mVBOCreated = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    int glUse = GL_STATIC_DRAW;
    if (mUsage == GHVBUsage::DYNAMIC) {
        glUse = GL_DYNAMIC_DRAW;
    }
    glBufferData(GL_ARRAY_BUFFER, mFormat->get()->getVertexSize()*sizeof(float)*mNumVerts, mBuffer, glUse);
    
    mVBOVersion = mVersion;
}

GHVertexStream* GHVertexStreamOGLVBO::clone(void) const
{
    GHVertexStreamOGLVBO* ret = new GHVertexStreamOGLVBO(mFormat, mNumVerts, mUsage);
    
    float* retBuffer = ret->lockWriteBuffer();
    memcpy(retBuffer, mBuffer, mFormat->get()->getVertexSize()*mNumVerts*sizeof(float));
    ret->unlockWriteBuffer();
    return ret;
}

