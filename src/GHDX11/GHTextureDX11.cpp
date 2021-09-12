// Copyright Golden Hammer Software
#include "GHTextureDX11.h"
#include "GHRenderDeviceDX11.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHTextureCreatorDX11.h"
#include "GHPlatform/GHDebugMessage.h"

GHTextureDX11::GHTextureDX11(GHRenderDeviceDX11& device,
							 Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex,
							 Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView,
							 GHEventMgr* eventMgr, const char* srcFileName,
							 GHTextureCreatorDX11* texCreator,
							 void* pixels, unsigned int width, unsigned int height, unsigned int depth,
							 bool usesMipmaps,
							 DXGI_FORMAT dxFormat)
: mD3DTex(d3dTex)
, mTextureView(textureView)
, mDevice(device)
, mEventMgr(eventMgr)
, mMessageListener(*this)
, mSrcFileName(srcFileName, GHString::CHT_COPY)
, mTexCreator(texCreator)
, mPixels(pixels)
, mWidth(width)
, mHeight(height)
, mDepth(depth)
, mDXFormat(dxFormat)
, mUsesMipmaps(usesMipmaps)
{
	if (mEventMgr) mEventMgr->registerListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
}

GHTextureDX11::~GHTextureDX11(void)
{
	if (mEventMgr) mEventMgr->unregisterListener(GHRenderProperties::DEVICEREINIT, mMessageListener);
	if (mPixels) {
		delete [] mPixels;
	}
}

void GHTextureDX11::setBindArgs(const GHTextureBindArgsDX11& bindArgs)
{
	mBindArgs = bindArgs;
}

void GHTextureDX11::bind(void)
{
	if (!mTextureView) return;

	if (mBindArgs.mIsVertex) {
		mDevice.getD3DContext()->VSSetShaderResources(
            mBindArgs.mRegister,
            1,
            mTextureView.GetAddressOf()
            );
	}
	else {
		mDevice.getD3DContext()->PSSetShaderResources(
            mBindArgs.mRegister,
            1,
            mTextureView.GetAddressOf()
            );
	}
}

bool GHTextureDX11::lockSurface(void** ret, unsigned int& pitch)
{
	if (mPixels) {
		*ret = mPixels;
		pitch = mWidth * mDepth;
		return true;
	}
	
	return lockD3DTex(ret, pitch);
}

bool GHTextureDX11::lockD3DTex(void** ret, unsigned int& pitch)
{
	if (mD3DTex == nullptr) {
		return false;
	}

	D3D11_TEXTURE2D_DESC textureDesc;
	mD3DTex->GetDesc(&textureDesc);
	assert(matchesDimensions(textureDesc));

	D3D11_TEXTURE2D_DESC stagingDesc = textureDesc;
	stagingDesc.Usage = D3D11_USAGE_STAGING;
	stagingDesc.BindFlags = 0;
	stagingDesc.MiscFlags = 0;
	stagingDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	HRESULT hr = mDevice.getD3DDevice()->CreateTexture2D(&stagingDesc, nullptr, &mStagingTex);
	if (FAILED(hr))
	{
		GHDebugMessage::outputString("Error in GHTextureDX11::lockD3DTex: failed to create staging texture");
		return false;
	}

	mDevice.getD3DContext()->CopyResource(mStagingTex.Get(), mD3DTex.Get());
	

	D3D11_MAPPED_SUBRESOURCE mapInfo;
	hr = mDevice.getD3DContext()->Map(mStagingTex.Get(), 0, D3D11_MAP_READ, 0, &mapInfo);
	if (FAILED(hr))
	{
		mStagingTex = nullptr;
		return false;
	}
	

	*ret = mapInfo.pData;
	pitch = mapInfo.RowPitch;

	return true;
}

bool GHTextureDX11::matchesDimensions(const D3D11_TEXTURE2D_DESC& desc) const
{
	if (desc.Width != mWidth) {
		return false;
	}
	if (desc.Height != mHeight) {
		return false;
	}
	return true;
}

bool GHTextureDX11::unlockSurface(void)
{
	if (mStagingTex != nullptr)
	{
		mDevice.getD3DContext()->Unmap(mStagingTex.Get(), 0);
		mStagingTex = nullptr;
	}

	return true;
}

bool GHTextureDX11::getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth)
{
	width = mWidth;
	height = mHeight;
	depth = mDepth;
	return true;
}

void GHTextureDX11::deleteSourceData(void)
{
}

void GHTextureDX11::setDeviceResources(Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex,
									   Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView)
{
	mD3DTex = d3dTex;
	mTextureView = textureView;
}

void GHTextureDX11::reinit(void)
{
	mD3DTex = nullptr;
	mTextureView = nullptr;
	
	if (mTexCreator)
	{
		// If we have saved pixels then reload from memory.
		if (mPixels)
		{
			mTexCreator->createTextureFromMemory(mD3DTex, mTextureView, mPixels, mWidth, mHeight, mDepth, mUsesMipmaps, mDXFormat, false);
		}
		// Otherwise reload from disk.
		else
		{
			void* pixels = 0;
			mTexCreator->createTexture(mSrcFileName.getChars(), mD3DTex, mTextureView, &pixels, mWidth, mHeight, mDepth, mUsesMipmaps, mDXFormat);
			// We didn't have saved pixels before, so don't save them now.
			if (pixels) 
			{
				delete[] pixels;
			}
		}
	}
}

DXGI_FORMAT GHTextureDX11::convertGHFormatToDXGI(GHTextureFormat::Enum ghFormat)
{
	if (ghFormat == GHTextureFormat::TF_DXT1)
	{
		return DXGI_FORMAT_BC1_UNORM;
	}
	if (ghFormat == GHTextureFormat::TF_DXT5)
	{
		return DXGI_FORMAT_BC3_UNORM;
	}
	if (ghFormat == GHTextureFormat::TF_RGB8)
	{
		// there is no 24 bit d3d texture format.
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	if (ghFormat == GHTextureFormat::TF_RGBA8)
	{
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	}
	return DXGI_FORMAT_R8G8B8A8_UNORM;
}