// Copyright Golden Hammer Software
#pragma once

#include "GHTexture.h"
#include "GHDX11Include.h"
#include "GHUtils/GHMessageHandler.h"
#include "GHString/GHString.h"
#include "GHTextureFormat.h"
#include "GHTextureBindArgsDX11.h"

class GHRenderDeviceDX11;
class GHEventMgr;
class GHTextureCreatorDX11;

class GHTextureDX11 : public GHTexture
{
public:
	GHTextureDX11(GHRenderDeviceDX11& device,
				  Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex,
				  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView,
				  GHEventMgr* eventMgr,
				  const char* srcFileName, GHTextureCreatorDX11* texCreator,
				  void* pixels, unsigned int width, unsigned int height, unsigned int depth,
				  bool usesMipmaps,
				  DXGI_FORMAT dxFormat);
	~GHTextureDX11(void);

	// We need some information outside of the standard bind() call in order to know what to do.
	void setBindArgs(const GHTextureBindArgsDX11& bindArgs);
	virtual void bind(void);

    virtual bool lockSurface(void** ret, unsigned int& pitch);
	virtual bool unlockSurface(void);
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth);
	virtual void deleteSourceData(void);

	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> getTextureView(void) { return mTextureView; }
	Microsoft::WRL::ComPtr<ID3D11Texture2D> getD3DTex(void) { return mD3DTex; }

	// used for overriding what was created without invalidating GHTexture handles.
	// useful for release/regain device handles.
	void setDeviceResources(Microsoft::WRL::ComPtr<ID3D11Texture2D> d3dTex,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> textureView);

	void reinit(void);

private:
	class MessageListener : public GHMessageHandler
	{
	public:
		MessageListener(GHTextureDX11& parent) : mParent(parent) {}
		virtual void handleMessage(const GHMessage& message) { mParent.reinit(); }

	private:
		GHTextureDX11& mParent;
	};

private:
	bool lockD3DTex(void** ret, unsigned int& pitch);
	bool matchesDimensions(const D3D11_TEXTURE2D_DESC& desc) const;

private:
	GHRenderDeviceDX11& mDevice;
	GHTextureBindArgsDX11 mBindArgs;
	GHEventMgr* mEventMgr;
	MessageListener mMessageListener;
	GHString mSrcFileName;
	GHTextureCreatorDX11* mTexCreator;

	Microsoft::WRL::ComPtr<ID3D11Texture2D> mD3DTex;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> mTextureView;

	//For locking the pixels for CPU read
	Microsoft::WRL::ComPtr<ID3D11Texture2D> mStagingTex;

	void* mPixels;
	unsigned int mWidth;
	unsigned int mHeight;
	unsigned int mDepth;
	DXGI_FORMAT mDXFormat;
	bool mUsesMipmaps;
};
