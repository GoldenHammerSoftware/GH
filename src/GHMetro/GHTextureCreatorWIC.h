// Copyright Golden Hammer Software
#pragma once 

#include "GHTextureCreatorDX11.h"
#include "GHPlatform/win32/GHWin32Include.h"
#include "wincodec.h"
#include <wrl/client.h>

class GHWindowsFileFinder;
class GHRenderDeviceDX11;

class GHTextureCreatorWIC : public GHTextureCreatorDX11
{
public:
	GHTextureCreatorWIC(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener);
	~GHTextureCreatorWIC(void);

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
	const GHWindowsFileFinder& mFileOpener;
	GHRenderDeviceDX11& mDevice;
	IWICImagingFactory* mImagingFactory;
};
