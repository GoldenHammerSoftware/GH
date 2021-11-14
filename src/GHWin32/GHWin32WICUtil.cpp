#include "GHWin32WICUtil.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHWindowsFileFinder.h"
#include <wrl/client.h>

GHWin32WICUtil::GHWin32WICUtil(const GHWindowsFileFinder& fileFinder)
	: mFileFinder(fileFinder)
{
	HRESULT hr = CoCreateInstance(CLSID_WICImagingFactory, NULL, CLSCTX_INPROC_SERVER,
		IID_IWICImagingFactory, (LPVOID*)&mImagingFactory);
	if (FAILED(hr) || !mImagingFactory) {
		GHDebugMessage::outputString("Failed to create imaging factory");
	}
}

GHWin32WICUtil::~GHWin32WICUtil(void)
{
	CoUninitialize();
}

bool GHWin32WICUtil::createTexture(const char* filename,
	void** pixels, unsigned int& width, unsigned int& height, unsigned int& depth,
	DXGI_FORMAT& dxFormat)
{
	// convert filename to wchar so we can interact with windows.
	const int BUF_SZ = 1024;
	wchar_t wcharFilename[BUF_SZ];
	if (!mFileFinder.createFilePath(filename, wcharFilename, BUF_SZ, true))
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
		mFileFinder.createFilePath(filename, wcharFilename, BUF_SZ, false);
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
	dxFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

	// copy the texture to a memory buffer.
	UINT stride = width * 4;
	UINT imageSize = stride * height;
	*pixels = new BYTE[imageSize];
	HRESULT copyRes = convertedFrame->CopyPixels(NULL, stride, imageSize, (BYTE*)*pixels);
	if (FAILED(copyRes)) {
		GHDebugMessage::outputString("Failed to copy pixels to mem buf");
		delete[] *pixels;
		*pixels = 0;
		return false;
	}

	depth = 32 / 8;
	return true;
}

