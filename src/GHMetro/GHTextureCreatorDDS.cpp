// Copyright Golden Hammer Software
#include "GHTextureCreatorDDS.h"
#include "GHWin32/GHWindowsFileFinder.h"
#include "DDSTextureLoader.h"
#include "GHRenderDeviceDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include <assert.h>

GHTextureCreatorDDS::GHTextureCreatorDDS(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener)
: mDevice(device)
, mFileOpener(fileOpener)
{
}

bool GHTextureCreatorDDS::createTexture(const char* filename, Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
	void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
	bool useMipmaps,
	DXGI_FORMAT& dxFormat)
{
	// we don't allow access to this info for dds textures.
	*pixels = 0;
	width = 0;
	height = 0;
	depth = 0;
	dxFormat = DXGI_FORMAT_UNKNOWN;

	const int BUF_SZ = 1024;
	wchar_t wcharFilename[BUF_SZ];
	if (!mFileOpener.createFilePath(filename, wcharFilename, BUF_SZ, true))
	{
		return false;
	}

    d3dTex = nullptr;
	textureView = nullptr;

	HRESULT texRes = GHDDSLoader::CreateDDSTextureFromFile(mDevice.getD3DDevice().Get(),
		wcharFilename,
		(ID3D11Resource**)d3dTex.GetAddressOf(),
		textureView.GetAddressOf());

	if (FAILED(texRes)) {
		// this generally means the texture does not exist.
		//GHDebugMessage::outputString("Failed to create dds texture from file %s", filename);
		d3dTex = nullptr;
		textureView = nullptr;
		return false;
	}
	return true;
}

bool GHTextureCreatorDDS::createTextureFromMemory(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
	void* pixels, unsigned int width, unsigned int height, unsigned int depth, 
	bool useMipmaps, DXGI_FORMAT dxFormat,
	bool okToDeleteImageBuf)
{
	// TODO.
	assert(false);
	return false;
}
