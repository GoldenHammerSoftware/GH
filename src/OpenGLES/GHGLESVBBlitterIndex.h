// Copyright 2010 Golden Hammer Software
#pragma once

#include "GHVBBlitter.h"
#include "GHGLESInclude.h"
#include "GHRenderDeviceReinitListener.h"

class GHEventMgr;

class GHGLESVBBlitterIndex : public GHVBBlitterIndex
{
public:
    GHGLESVBBlitterIndex(GHEventMgr& eventMgr, unsigned int numIndices, bool vaoSupported);
    ~GHGLESVBBlitterIndex(void);
    
    virtual void prepareVB(GHVertexBuffer& vb);
    virtual void endVB(GHVertexBuffer& vb);
    virtual void blit(void);
    
    virtual unsigned short* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const unsigned short* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;
    
private:
    void createVBO(void);
    void onDeviceReinit(void);

private:
	bool mVAOSupported{ false };

    unsigned short* mBuffer;
    
    GLuint mVBO;
    bool mVBOCreated;
    
    // a way to keep track of whether our vbo is up to date.
    unsigned int mVersion;
    unsigned int mVBOVersion;
    
#if GH_ES_VERSION >= 3
	// VAO stores the vertex layout so we don't have to repeatedly call glVertexAttribPointer
	// only available on es 3+.
	GLuint mVAO{ 0 };
	bool mVAOCreated{ false };
#endif

private:
	GHRenderDeviceReinitListener<GHGLESVBBlitterIndex> mDeviceListener;
	friend class GHRenderDeviceReinitListener<GHGLESVBBlitterIndex>; // to call private reinit.
};
