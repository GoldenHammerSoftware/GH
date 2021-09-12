// Copyright Golden Hammer Software
#pragma once

#include "GHShaderDX11.h"
#include "GHDX11Include.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHString.h"

class GHEventMgr;

// A concrete dx11 shader means we wrap the actual ID3D11VertexShader
// GHShaderDX11 is the interface for all shaders, and we moved this
//  code into concrete implementation in order to allow for GHMultiShader to exist.
class GHShaderConcreteDX11 : public GHShaderDX11
{
public:
	GHShaderConcreteDX11(GHRenderDeviceDX11& device, char* data, size_t dataLen, 
		GHShaderDX11*& activeVS, GHResourcePtr<GHShaderParamListDX11>* params,
		GHEventMgr& eventMgr, const char* shaderName);
	~GHShaderConcreteDX11(void);

	// we expect one of these two functions to be called after construction.
	// they return false if a compile failed.
	bool initAsVS(void);
	bool initAsPS(void);

	virtual void bind(void);

	virtual void getBytecode(const char*& data, size_t& dataLen) const;
	virtual const GHShaderParamListDX11& getParamList(void) const { return *mParams->get(); }

	void reinit(void);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHShaderConcreteDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHShaderConcreteDX11& mParent;
	};

private:
	GHRenderDeviceDX11& mDevice;
	GHEventMgr& mEventMgr;
	MessageListener mMessageListener;
	GHString mShaderName; // for debugging.

	GHResourcePtr<GHShaderParamListDX11>* mParams; // assumed to not be null.
	// we expect to have either a vertex shader or a pixel shader but not both.
	Microsoft::WRL::ComPtr<ID3D11VertexShader> mVertexShader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> mPixelShader;
	char* mData;
	size_t mDataLen;

	// The globally active VS.  if we have a mVertexShader, we update mActiveVS to point to (this).
	GHShaderDX11** mActiveVS;
};
