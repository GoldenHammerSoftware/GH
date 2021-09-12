// Copyright 2010 Golden Hammer Software
#include "GHVBBlitterIndexOGL.h"
#include "GHGLErrorReporter.h"
#include "GHVertexBuffer.h"

GHVBBlitterIndexOGL::GHVBBlitterIndexOGL(unsigned int numIndices)
: GHVBBlitterIndex(numIndices)
, mVersion(1)
, mVBOVersion(0)
, mVBOCreated(false)
{
    mBuffer = new unsigned short[mNumIndices];
}

GHVBBlitterIndexOGL::~GHVBBlitterIndexOGL(void)
{
    delete [] mBuffer;
    if (mVBOCreated) 
    {
		glDeleteBuffers(1, &mVBO);
	}
}

void GHVBBlitterIndexOGL::prepareVB(GHVertexBuffer& vb)
{
    const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
    for (size_t i = 0; i < streams.size(); ++i)
    {
        streams[i]->get()->prepareVB(i, 0);
    }
    
    if (mVersion != mVBOVersion)
    {
        createVBO();
    }
    
    // gles version binds vb here.  gl version is binding in blit instead.
    // djw thinks this feels odd and is worth looking into.
}

void GHVBBlitterIndexOGL::endVB(GHVertexBuffer& vb)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GHVBBlitterIndexOGL::blit(void)
{
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO);
	glDrawElements(GL_TRIANGLES, mActiveIndices, GL_UNSIGNED_SHORT, 0);
	GHGLErrorReporter::checkAndReportError("glDrawElements");
}

unsigned short* GHVBBlitterIndexOGL::lockWriteBuffer(void)
{
    return mBuffer;
}

void GHVBBlitterIndexOGL::unlockWriteBuffer(void)
{
    mVersion++;
}

const unsigned short* GHVBBlitterIndexOGL::lockReadBuffer(void) const
{
    return mBuffer; 
}

void GHVBBlitterIndexOGL::unlockReadBuffer(void) const
{
    
}

void GHVBBlitterIndexOGL::createVBO(void)
{
    if (!mVBOCreated)
    {
        glGenBuffers(1, &mVBO);
		GHGLErrorReporter::checkAndReportError("glGenBuffers");

        mVBOCreated = true;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO);
    GHGLErrorReporter::checkAndReportError("glBindBuffer - index");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices*sizeof(unsigned short), mBuffer, GL_STATIC_DRAW);
    GHGLErrorReporter::checkAndReportError("glBufferData - index");

    mVBOVersion = mVersion;
}


