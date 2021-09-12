// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVertexStream.h"
#include "GHGLESInclude.h"
#include "GHRenderDeviceReinitListener.h"

class GHEventMgr;

class GHGLESVertexStreamVBO : public GHVertexStream
{
public:
    GHGLESVertexStreamVBO(GHEventMgr& eventMgr, GHVertexStreamFormatPtr* format, unsigned int numVerts, GHVBUsage::Enum usage);
    ~GHGLESVertexStreamVBO(void);
    
	// todo: formatOverride should be const.
    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride);
    virtual void endVB(int streamIdx);
    
    virtual float* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const float* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;
    
    virtual GHVertexStream* clone(void) const;
    
	// check to see if anything has changed that would trigger a new vao.
	bool needsReprepare(void) const;

private:
    void createVBO(void);
    void onDeviceReinit(void);

	void applyVertexFormat(const GHVertexStreamFormat& format);
	void removeVertexFormat(void);

private:
	GHRenderDeviceReinitListener<GHGLESVertexStreamVBO> mDeviceListener;
	friend class GHRenderDeviceReinitListener<GHGLESVertexStreamVBO>; // to call private reinit.

	GHEventMgr& mEventMgr;
	
	float* mBuffer;
	GHVBUsage::Enum mVBUsage;

    GLuint mVBO;
    // a way to keep track of whether our vbo is up to date.
    unsigned int mVersion;
    unsigned int mVBOVersion;
	bool mVBOCreated{ false };
};
