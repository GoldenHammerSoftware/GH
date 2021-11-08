// Copyright Golden Hammer Software
#pragma once 

#include "GHTextureCreatorDX11.h"
#include "GHPlatform/win32/GHWin32Include.h"
#include <wrl/client.h>
#include "GHWin32/GHWin32WICUtil.h"

class GHWindowsFileFinder;
class GHRenderDeviceDX11;

class GHTextureCreatorWICDX11 : public GHTextureCreatorDX11
{
public:
	GHTextureCreatorWICDX11(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener);
	~GHTextureCreatorWICDX11(void);

	virtual bool createTexture(const char* filename,
		Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
		void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
		bool useMipmaps,
		DXGI_FORMAT& dxFormat) override;

	virtual bool createTextureFromMemory(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
		void* pixels, unsigned int width, unsigned int height, unsigned int depth,
		bool useMipmaps,
		DXGI_FORMAT dxFormat, bool okToDeleteImageBuf) override;

private:
	GHRenderDeviceDX11& mDevice;
	GHWin32WICUtil mWICUtil;
};
