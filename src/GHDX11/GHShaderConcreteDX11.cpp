// Copyright Golden Hammer Software
#include "GHShaderConcreteDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHShaderParamListDX11.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"

GHShaderConcreteDX11::GHShaderConcreteDX11(GHRenderDeviceDX11& device, 
										   char* data, size_t dataLen, GHShaderDX11*& activeVS, 
										   GHResourcePtr<GHShaderParamListDX11>* params,
										   GHEventMgr& eventMgr, const char* shaderName)
: mDevice(device)
, mData(data)
, mDataLen(dataLen)
, mPixelShader(0)
, mVertexShader(0)
, mActiveVS(&activeVS)
, mParams(0)
, mEventMgr(eventMgr)
, mMessageListener(*this)
, mShaderName(shaderName, GHString::CHT_COPY)
{
	GHResource::changePointer((GHRefCounted*&)mParams, params);
	assert(mParams);

	mEventMgr.registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

bool GHShaderConcreteDX11::initAsVS(void)
{
	mVertexShader = nullptr;
	HRESULT hres = mDevice.getD3DDevice()->CreateVertexShader(
            mData,
            mDataLen,
            nullptr,
            mVertexShader.GetAddressOf()
            );
	if (FAILED(hres))
	{
		return false;
	}
	return true;
}

bool GHShaderConcreteDX11::initAsPS(void)
{
	mPixelShader = nullptr;
	HRESULT hres = mDevice.getD3DDevice()->CreatePixelShader(
					mData,
					mDataLen,
					nullptr,
					mPixelShader.GetAddressOf()
					);
	if (FAILED(hres)) 
	{
		return false;
	}

	return true;
}

GHShaderConcreteDX11::~GHShaderConcreteDX11(void)
{
	delete [] mData;
	GHResource::changePointer((GHRefCounted*&)mParams, 0);
	mEventMgr.unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

void GHShaderConcreteDX11::bind(void)
{
	if (mVertexShader)
	{
		mDevice.getD3DContext()->VSSetShader(
			mVertexShader.Get(),
			nullptr,
			0
			);
		*mActiveVS = this;
	}
	if (mPixelShader)
	{
		mDevice.getD3DContext()->PSSetShader(
			mPixelShader.Get(),
			nullptr,
			0
			);
	}
}

void GHShaderConcreteDX11::getBytecode(const char*& data, size_t& dataLen) const
{
	data = mData;
	dataLen = mDataLen;
}

void GHShaderConcreteDX11::reinit(void)
{
	if (mPixelShader)
	{
		mPixelShader = nullptr;
		initAsPS();
	}
	if (mVertexShader)
	{
		mVertexShader = nullptr;
		initAsVS();
	}
}
