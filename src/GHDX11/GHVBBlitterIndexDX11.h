// Copyright Golden Hammer Software
#pragma once

#include "GHVBBlitter.h"
#include "GHDX11Include.h"
#include "GHVertexComponent.h"
#include <map>
#include "GHVBUsage.h"
#include "GHUtils/GHMessageHandler.h"
#include <vector>
#include "GHVertexStreamFormat.h"

class GHRenderDeviceDX11;
class GHShaderDX11;
class GHEventMgr;

class GHVBBlitterIndexDX11 : public GHVBBlitterIndex
{
public:
	GHVBBlitterIndexDX11(unsigned int numIndices, GHRenderDeviceDX11& device, GHShaderDX11*& activeVS, 
		GHVBUsage::Enum usage, GHEventMgr& eventMgr);
    ~GHVBBlitterIndexDX11(void);

	virtual void prepareVB(GHVertexBuffer& vb);
    virtual void endVB(GHVertexBuffer& vb);
    virtual void blit(void);

    virtual unsigned short* lockWriteBuffer(void);
    virtual void unlockWriteBuffer(void);
    virtual const unsigned short* lockReadBuffer(void) const;
    virtual void unlockReadBuffer(void) const;

	void reinit(void);

private:
	void createHardwareBuffer(void);
	void createIED(GHVertexBuffer& vb);
	int applyStreamComponentsToIED(const std::vector<GHVertexStreamFormat::ComponentEntry>& comps, int streamIdx, int localCompIdx);
	Microsoft::WRL::ComPtr<ID3D11InputLayout> getInputLayout(GHShaderDX11* activeVS);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHVBBlitterIndexDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHVBBlitterIndexDX11& mParent;
	};

private:
	GHRenderDeviceDX11& mDevice;
	GHShaderDX11*& mActiveVS;
	GHVBUsage::Enum mUsage;
	GHEventMgr& mEventMgr;
	MessageListener mMessageListener;

	Microsoft::WRL::ComPtr<ID3D11Buffer> mHardwareBuffer;
	// we keep a memory copy of the verts for lock read and for partial writes.
	unsigned short* mMemoryBuffer;

	// triangle list, triangle strip, etc.
	// in our case, always triangle list.
	D3D11_PRIMITIVE_TOPOLOGY mD3DPrimitiveType;
	// array describing what is contained in the buffer.
	D3D11_INPUT_ELEMENT_DESC* mD3DIED;
	// the number of entries in mD3DIED
	unsigned int mInputElementCount;

	// We need a different input layout for each vertex shader, so we keep a mapping.
	typedef std::map< const GHShaderDX11*, Microsoft::WRL::ComPtr<ID3D11InputLayout> > LayoutMap;
	LayoutMap mInputLayouts;
};
