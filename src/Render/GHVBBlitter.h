// Copyright Golden Hammer Software
#pragma once

#include "GHPlatform/GHRefCounted.h"
#include <assert.h>

class GHVertexBuffer;

// class to describe how a vb does the actual draw call
// the streams are set up outside of this class.
// example: GHBufferBlitterOGLVBOIndexBuffer
class GHVBBlitter
{
public:
    enum Type
    {
        BT_INDEX=0,
        BT_UNKNOWN,
    };
    
public:
    virtual ~GHVBBlitter(void) {}

    virtual void prepareVB(GHVertexBuffer& vb) = 0;
    virtual void endVB(GHVertexBuffer& vb) = 0;
    virtual void blit(void) = 0;
    
    Type getType(void) const { return mType; }
    
protected:
    // used for some runtime type validation to determine if we 
    //  can cast to GHBufferBlitterIndex.
	Type mType{ BT_UNKNOWN };
};

// A buffer blitter that wraps an index buffer.
class GHVBBlitterIndex : public GHVBBlitter
{
public:
	GHVBBlitterIndex(unsigned int numIndices) : mNumIndices(numIndices), mActiveIndices(numIndices) { mType = BT_INDEX; }
    
    unsigned int getNumIndices(void) const { return mNumIndices; }
	unsigned int getActiveIndices(void) const { return mActiveIndices; }

    virtual unsigned short* lockWriteBuffer(void) = 0;
    virtual void unlockWriteBuffer(void) = 0;
    
    virtual const unsigned short* lockReadBuffer(void) const = 0;
    virtual void unlockReadBuffer(void) const = 0;

	// sometimes we want to draw less than the full buffer without resizing.
	// activeIndicies must be < mNumIndicies.
	void setActiveIndices(unsigned int activeIndices)
	{
		assert(activeIndices <= mNumIndices);
		mActiveIndices = activeIndices;
	}
    
protected:
    unsigned int mNumIndices;
	unsigned int mActiveIndices;
};

typedef GHRefCountedType<GHVBBlitter> GHVBBlitterPtr;
