// Copyright Golden Hammer Software
#include "GHTextureCreatorWICDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHWin32/GHWindowsFileFinder.h"
#include "GHDX11/GHRenderDeviceDX11.h"
#include "GHDX11/DDSTextureLoader.h"
#include "GHWin32/GHDXGIUtil.h"

GHTextureCreatorWICDX11::GHTextureCreatorWICDX11(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileFinder)
: mDevice(device)
, mWICUtil(fileFinder)
{
}

GHTextureCreatorWICDX11::~GHTextureCreatorWICDX11(void)
{
}

bool GHTextureCreatorWICDX11::createTexture(const char* filename, Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dtex,
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
	void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
	bool useMipmaps,
	DXGI_FORMAT& dxFormat)
{
	*pixels = 0;
	width = 0;
	height = 0;
	depth = 0;

    d3dtex = nullptr;
	textureView = nullptr;

	bool wicWorked = mWICUtil.createTexture(filename, pixels, width, height, depth, dxFormat);
	if (!wicWorked)
	{
		return false;
	}

	bool createdD3DTexture = createTextureFromMemory(d3dtex, textureView,
		*pixels, width, height, depth, useMipmaps, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (!createdD3DTexture) {
		delete [] pixels;
	}
	// pixels is now owned by the texture.

	return createdD3DTexture;
}

bool GHTextureCreatorWICDX11::createTextureFromMemory(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
												  Microsoft::WRL::ComPtr<ID3D11ShaderResourceView>& textureView,
												  void* imageBuf, unsigned int width, unsigned int height, unsigned int depth,
												  bool useMipmaps,
												  DXGI_FORMAT dxFormat, bool okToDeleteImageBuf)
{
	HRESULT hr;

	// See if format is supported for auto-gen mipmaps (varies by feature level)
	bool autogen = false;
	if (useMipmaps)
	{
		if (mDevice.getD3DContext() != 0 /*&& textureView != 0*/) // Must have context and shader-view to auto generate mipmaps
		{
			UINT fmtSupport = 0;
			hr = mDevice.getD3DDevice()->CheckFormatSupport(dxFormat, &fmtSupport);
			if (SUCCEEDED(hr) && (fmtSupport & D3D11_FORMAT_SUPPORT_MIP_AUTOGEN))
			{
				autogen = true;
			}
		}
	}

	// create the DX texture from the source buffer.
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.MipLevels = (autogen) ? 0 : 1;
	desc.ArraySize = 1;
	desc.Format = dxFormat;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.BindFlags = (autogen) ? (D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET) : (D3D11_BIND_SHADER_RESOURCE);
	desc.CPUAccessFlags = 0;
	desc.MiscFlags = (autogen) ? D3D11_RESOURCE_MISC_GENERATE_MIPS : 0;

	UINT stride = width * depth;
	UINT imageSize = stride * height;

	D3D11_SUBRESOURCE_DATA initData;
	initData.pSysMem = imageBuf;

	size_t ddsSize = GHDXGIUtil::bitsPerPixel(dxFormat);
	if (ddsSize != 0)
	{
		// the dxt formats have lots of special rules.
		// we just call through to the dds loader to calculate the important info for us.
		size_t numBytes = 0;
		size_t rowBytes = 0;
		size_t numRows = 0;
		GHDXGIUtil::getSurfaceInfo(width, height, dxFormat,
			&numBytes, &rowBytes, &numRows);

		initData.SysMemPitch = static_cast<UINT>(rowBytes);
		initData.SysMemSlicePitch = static_cast<UINT>(numBytes);
	}
	else
	{
		// not a dds.
		initData.SysMemPitch = stride;
		initData.SysMemSlicePitch = static_cast<UINT>(imageSize);
	}

	HRESULT createTexRes = mDevice.getD3DDevice()->CreateTexture2D(&desc, (autogen) ? nullptr : &initData, (ID3D11Texture2D**)d3dTex.GetAddressOf());
	if (FAILED(createTexRes)) {
		GHDebugMessage::outputString("Failed to create d3d tex");
		if (okToDeleteImageBuf) delete[] imageBuf;
		return false;
	}

	// Create the texture view so we can pass it to a sampler
	D3D11_SHADER_RESOURCE_VIEW_DESC textureViewDesc;
	ZeroMemory(&textureViewDesc, sizeof(textureViewDesc));
	textureViewDesc.Format = desc.Format;
	textureViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	textureViewDesc.Texture2D.MipLevels = (autogen) ? -1 : 1;
	textureViewDesc.Texture2D.MostDetailedMip = 0;

	if (FAILED(
		mDevice.getD3DDevice()->CreateShaderResourceView(
			d3dTex.Get(),
			&textureViewDesc,
			textureView.GetAddressOf()
		)
	))
	{
		GHDebugMessage::outputString("Failed to create shader resource view");
		if (okToDeleteImageBuf) delete[] imageBuf;
		return false;
	}

	if (autogen)
	{
		assert(mDevice.getD3DContext() != 0);
		mDevice.getD3DContext()->UpdateSubresource(d3dTex.Get(), 0, nullptr, imageBuf, static_cast<UINT>(stride), static_cast<UINT>(imageSize));
		mDevice.getD3DContext()->GenerateMips(textureView.Get());
	}

	return true;
}
