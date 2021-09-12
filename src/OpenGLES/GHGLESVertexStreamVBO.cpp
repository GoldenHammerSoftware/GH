// Copyright 2010 Golden Hammer Software
#include "GHGLESVertexStreamVBO.h"
#include "GHGLESErrorReporter.h"
#include <string.h>
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"

GHGLESVertexStreamVBO::GHGLESVertexStreamVBO(GHEventMgr& eventMgr, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage)
: GHVertexStream(format)
, mVersion(1)
, mVBOVersion(0)
, mVBOCreated(false)
, mEventMgr(eventMgr)
, mDeviceListener(eventMgr, *this)
, mVBUsage(usage)
{
    mNumVerts = numVerts;
    mBuffer = new float[mNumVerts * format->get()->getVertexSize()];
}

GHGLESVertexStreamVBO::~GHGLESVertexStreamVBO(void)
{
    delete [] mBuffer;
    if (mVBOCreated)
    {
        glDeleteBuffers(1, &mVBO);
    }
}

bool GHGLESVertexStreamVBO::needsReprepare(void) const
{
	return (mVersion != mVBOVersion);
}

void GHGLESVertexStreamVBO::prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride)
{
    GHVertexStreamFormat* format = formatOverride ? formatOverride : mFormat->get();
    if (mVersion != mVBOVersion)
    {
        createVBO();
    }

	glBindBuffer(GL_ARRAY_BUFFER, mVBO);
	applyVertexFormat(*format);

    GHGLESErrorReporter::checkAndReportError("GHGLESVertexStreamVBO::prepareVB");
}

void GHGLESVertexStreamVBO::endVB(int streamIdx)
{
	removeVertexFormat();
	glBindBuffer(GL_ARRAY_BUFFER, 0);
    GHGLESErrorReporter::checkAndReportError("endVB - glBindBuffer");
}

void GHGLESVertexStreamVBO::applyVertexFormat(const GHVertexStreamFormat& format)
{
	unsigned short vertSize = format.getVertexSize();
	const std::vector<GHVertexStreamFormat::ComponentEntry>& components = format.getComponents();
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
			vertSize * sizeof(float),
			(GLvoid*)(comp.mOffset * sizeof(float)));
	}
}

void GHGLESVertexStreamVBO::removeVertexFormat(void)
{
	// todo?: don't disable unless the next buffer isn't going to use the types.
	const std::vector<GHVertexStreamFormat::ComponentEntry>& components = mFormat->get()->getComponents();
	for (int i = 0; i < components.size(); ++i)
	{
		glDisableVertexAttribArray(components[i].mComponent);
	}
	GHGLESErrorReporter::checkAndReportError("endVB - glDisableVertexAttribArray");
}

float* GHGLESVertexStreamVBO::lockWriteBuffer(void)
{
    return mBuffer;
}

void GHGLESVertexStreamVBO::unlockWriteBuffer(void)
{
    mVersion++;
}

const float* GHGLESVertexStreamVBO::lockReadBuffer(void) const
{
    return mBuffer;
}

void GHGLESVertexStreamVBO::unlockReadBuffer(void) const
{
    
}

void GHGLESVertexStreamVBO::createVBO(void)
{
    if (!mVBOCreated)
    {
        glGenBuffers(1, &mVBO);
        GHGLESErrorReporter::checkAndReportError("glGenBuffers - GHVertexStreamOGLVBO");
        mVBOCreated = true;
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, mVBO);
    int bufferSize = mFormat->get()->getVertexSize()*sizeof(float)*mNumVerts;
	const GLenum vbUseType = (mVBUsage == GHVBUsage::DYNAMIC) ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW;
    glBufferData(GL_ARRAY_BUFFER, bufferSize, 0, vbUseType);
    glBufferSubData(GL_ARRAY_BUFFER, 0, bufferSize, mBuffer);

    mVBOVersion = mVersion;
}

GHVertexStream* GHGLESVertexStreamVBO::clone(void) const
{
    GHGLESVertexStreamVBO* ret = new GHGLESVertexStreamVBO(mEventMgr, mFormat, mNumVerts, mVBUsage);
    
    float* retBuffer = ret->lockWriteBuffer();
    memcpy(retBuffer, mBuffer, mFormat->get()->getVertexSize()*mNumVerts*sizeof(float));
    ret->unlockWriteBuffer();
    return ret;
}

void GHGLESVertexStreamVBO::onDeviceReinit(void)
{
    mVersion++;
    mVBOCreated = false;
}
