// Copyright Golden Hammer Software
#pragma once

#include "GHVertexStream.h"
#include "GHDX11Include.h"
#include "GHUtils/GHMessageHandler.h"

class GHRenderDeviceDX11;
class GHEventMgr;

class GHVertexStreamDX11 : public GHVertexStream
{
public:
    GHVertexStreamDX11(GHVertexStreamFormatPtr* format, unsigned int numVerts, GHRenderDeviceDX11& device, 
		GHVBUsage::Enum usage, GHEventMgr& eventMgr);
    virtual ~GHVertexStreamDX11(void);
    
    // set up the vertex components for use by the gpu.
    virtual void prepareVB(int streamIdx, GHVertexStreamFormat* formatOverride);
    virtual void endVB(int streamIdx);
    
    virtual float* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    
    virtual const float* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;
    
    virtual GHVertexStream* clone(void) const;
    
	void reinit(void);

private:
	void createHardwareBuffer(void);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHVertexStreamDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHVertexStreamDX11& mParent;
	};

private:
	MessageListener mMessageListener;
	GHRenderDeviceDX11& mDevice;
	GHEventMgr& mEventMgr;
	GHVBUsage::Enum mUsage;
	Microsoft::WRL::ComPtr<ID3D11Buffer> mHardwareBuffer;
	// we keep a memory copy of the verts for lock read and for partial writes.
	void* mMemoryBuffer;
};