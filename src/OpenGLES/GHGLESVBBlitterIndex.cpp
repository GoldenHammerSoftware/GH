// Copyright 2010 Golden Hammer Software
#include "GHGLESVBBlitterIndex.h"
#include "GHGLESErrorReporter.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHGLESVertexStreamVBO.h"
#include "GHVertexBuffer.h"

GHGLESVBBlitterIndex::GHGLESVBBlitterIndex(GHEventMgr& eventMgr, unsigned int numIndices, bool vaoSupported)
: GHVBBlitterIndex(numIndices)
, mVersion(1)
, mVBOVersion(0)
, mVBOCreated(false)
, mDeviceListener(eventMgr, *this)
, mVAOSupported(vaoSupported)
{
    mBuffer = new unsigned short[mNumIndices];
}

GHGLESVBBlitterIndex::~GHGLESVBBlitterIndex(void)
{
    delete [] mBuffer;
    if (mVBOCreated) 
    {
		glDeleteBuffers(1, &mVBO);
	}

#if GH_ES_VERSION >= 3
	if (mVAOSupported && mVAOCreated)
	{
		glDeleteVertexArrays(1, &mVAO);
	}
#endif
}

void GHGLESVBBlitterIndex::prepareVB(GHVertexBuffer& vb)
{
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();

#if GH_ES_VERSION >= 3
	if (mVAOSupported)
	{
		bool needNewVAO = false;
		for (size_t i = 0; i < streams.size(); ++i)
		{
			// Assume all streams are GHGLESVertexStreamVBO.
			// I didn't feel like making an interface for GHGLESVertexStream and this needsReprepare is totally a VAO concept.
			needNewVAO |= ((GHGLESVertexStreamVBO*)streams[i]->get())->needsReprepare();
		}
		needNewVAO |= (mVersion != mVBOVersion);

		if (needNewVAO)
		{
			if (mVAOCreated)
			{
				glDeleteVertexArrays(1, &mVAO);
			}
			glGenVertexArrays(1, &mVAO);
			GHGLESErrorReporter::checkAndReportError("glGenVertexArrays - createVAO");
			mVAOCreated = true;
			glBindVertexArray(mVAO);

			for (size_t i = 0; i < streams.size(); ++i)
			{
				streams[i]->get()->prepareVB(i, 0);
			}

			if (mVersion != mVBOVersion)
			{
				createVBO();
			}
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO);
		}
		else
		{
			glBindVertexArray(mVAO);
		}
		return;
	}
#endif
	// vao not supported, brute force it.
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->prepareVB(i, 0);
	}

	if (mVersion != mVBOVersion)
	{
		createVBO();
	}
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO);
}

void GHGLESVBBlitterIndex::endVB(GHVertexBuffer& vb)
{
#if GH_ES_VERSION >= 3
	if (mVAOSupported)
	{
		glBindVertexArray(0);
		return;
	}
#endif
	const std::vector<GHVertexStreamPtr*>& streams = vb.getStreams();
	for (size_t i = 0; i < streams.size(); ++i)
	{
		streams[i]->get()->endVB(i);
	}

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void GHGLESVBBlitterIndex::blit(void)
{
	glDrawElements(GL_TRIANGLES, mActiveIndices, GL_UNSIGNED_SHORT, 0);
	GHGLESErrorReporter::checkAndReportError("glDrawElements");
}

unsigned short* GHGLESVBBlitterIndex::lockWriteBuffer(void)
{
    return mBuffer;
}

void GHGLESVBBlitterIndex::unlockWriteBuffer(void)
{
    mVersion++;
}

const unsigned short* GHGLESVBBlitterIndex::lockReadBuffer(void) const
{
    return mBuffer; 
}

void GHGLESVBBlitterIndex::unlockReadBuffer(void) const
{
    
}

void GHGLESVBBlitterIndex::createVBO(void)
{
    if (!mVBOCreated)
    {
        glGenBuffers(1, &mVBO);
		GHGLESErrorReporter::checkAndReportError("glGenBuffers");
        
        mVBOCreated = true;
    }
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mVBO);
    GHGLESErrorReporter::checkAndReportError("glBindBuffer - index");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mNumIndices*sizeof(unsigned short), mBuffer, GL_STATIC_DRAW);
    GHGLESErrorReporter::checkAndReportError("glBufferData - index");
    
    mVBOVersion = mVersion;
}

void GHGLESVBBlitterIndex::onDeviceReinit(void)
{
    mVersion++;
	// our buffer objects have been destroyed.
    mVBOCreated = false;
#if GH_ES_VERSION >= 3
	mVAOCreated = false;
#endif
}
