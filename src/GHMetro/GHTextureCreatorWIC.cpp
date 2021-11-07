// Copyright Golden Hammer Software
#include "GHTextureCreatorWIC.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHWin32/GHWindowsFileFinder.h"
#include "GHRenderDeviceDX11.h"
#include "DDSTextureLoader.h"

GHTextureCreatorWIC::GHTextureCreatorWIC(GHRenderDeviceDX11& device, const GHWindowsFileFinder& fileOpener)
: mFileOpener(fileOpener)
, mDevice(device)
{
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
        IID_IWICImagingFactory, (LPVOID*)&mImagingFactory);
	if (FAILED(hr) || !mImagingFactory) {
		GHDebugMessage::outputString("Failed to create imaging factory");
	}
}

GHTextureCreatorWIC::~GHTextureCreatorWIC(void)
{
	CoUninitialize();
}

bool GHTextureCreatorWIC::createTexture(const char* filename, Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dtex,
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

	// convert filename to wchar so we can interact with windows.
	const int BUF_SZ = 1024;
	wchar_t wcharFilename[BUF_SZ];
	if (!mFileOpener.createFilePath(filename, wcharFilename, BUF_SZ, true))
	{
		//GHDebugMessage::outputString("Failed to create file path in GHTextureCreatorWIC for %s", filename);
		return false;
	}

	// make a decoder for the given file.
	Microsoft::WRL::ComPtr<IWICBitmapDecoder> decoder = 0;
	HRESULT hr = mImagingFactory->CreateDecoderFromFilename(wcharFilename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
	if (FAILED(hr) || !decoder) {
		// this generally means the file does not exist. (ie looking for .jpg when it is .png)
		//GHDebugMessage::outputString("Failed to find decoder for %s", filename);

		// failed to find in the read only dir, look in the write dir.
		mFileOpener.createFilePath(filename, wcharFilename, BUF_SZ, false);
		hr = mImagingFactory->CreateDecoderFromFilename(wcharFilename, NULL, GENERIC_READ, WICDecodeMetadataCacheOnDemand, decoder.GetAddressOf());
		if (FAILED(hr) || !decoder) {
			return false;
		}
	}

	// make sure the source image has at least one frame.
    UINT frameCount = 0;
    if (FAILED(decoder->GetFrameCount(&frameCount)))
    {
		GHDebugMessage::outputString("Failed to get the frame count from source image.");
		return false;
    }
	if (frameCount < 1) {
		GHDebugMessage::outputString("No frames found in source image.");
		return false;
	}

	// get frame 0, since we don't support multi-frame images.
	Microsoft::WRL::ComPtr<IWICBitmapFrameDecode> srcFrame;
	if (FAILED(decoder->GetFrame(0, srcFrame.GetAddressOf())))
	{
		GHDebugMessage::outputString("Failed to get frame 0 for source image.");
		return false;
	}
	if (FAILED(srcFrame->GetSize(&width, &height)))
	{
		GHDebugMessage::outputString("Failed to get the dimensions of the source frame.");
		return false;
	}
	
	// convert the source texture to a RGBA32 format d3d can understand.
	// todo: support RGB textures too.
    Microsoft::WRL::ComPtr<IWICFormatConverter> convertedFrame;
	if (FAILED(mImagingFactory->CreateFormatConverter(convertedFrame.GetAddressOf())) || !convertedFrame)
	{
		GHDebugMessage::outputString("Failed to create format converter.");
		return false;
	}
	HRESULT convertRes = convertedFrame->Initialize(srcFrame.Get(),
													GUID_WICPixelFormat32bppRGBA,
													WICBitmapDitherTypeNone,
													NULL, // The desired palette
													0, // The desired alpha threshold
													WICBitmapPaletteTypeCustom);
	if (FAILED(convertRes)) {
		GHDebugMessage::outputString("Failed to convert texture to RGBA");
		return false;
	}

	// copy the texture to a memory buffer.
	UINT stride = width*4;
	UINT imageSize = stride*height;
	BYTE* imageBuf = new BYTE[imageSize];
	HRESULT copyRes = convertedFrame->CopyPixels(NULL, stride, imageSize, imageBuf);
	if (FAILED(copyRes)) {
		GHDebugMessage::outputString("Failed to copy pixels to mem buf");
		delete [] imageBuf;
		return false;
	}

	depth = 32 / 8;

	bool createdD3DTexture = createTextureFromMemory(d3dtex, textureView,
		imageBuf, width, height, depth, useMipmaps, DXGI_FORMAT_R8G8B8A8_UNORM, true);
	dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	if (!createdD3DTexture) {
		delete [] imageBuf;
	}
	else {
		*pixels = imageBuf;
		// moved the ownership of the imagebuf to the caller.
		//delete [] imageBuf;
	}

	return createdD3DTexture;
}

bool GHTextureCreatorWIC::createTextureFromMemory(Microsoft::WRL::ComPtr<ID3D11Texture2D>& d3dTex,
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

	size_t ddsSize = GHDDSLoader::BitsPerPixel(dxFormat);
	if (ddsSize != 0)
	{
		// the dxt formats have lots of special rules.
		// we just call through to the dds loader to calculate the important info for us.
		size_t numBytes = 0;
		size_t rowBytes = 0;
		size_t numRows = 0;
		GHDDSLoader::GetSurfaceInfo(width, height, dxFormat,
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
