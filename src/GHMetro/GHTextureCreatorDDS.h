// Copyright Golden Hammer Software
#pragma once

#include "GHDX11/GHTextureCreatorDX11.h"

class GHRenderDeviceDX11;
class GHWindowsFileFinder;

class GHTextureCreatorDDS : public GHTextureCreatorDX11
{
public:
	GHTextureCreatorDDS(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener);

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
	const GHWindowsFileFinder& mFileOpener;
};
