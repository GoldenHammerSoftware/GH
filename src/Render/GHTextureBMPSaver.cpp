#include "GHTextureBMPSaver.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHTexture.h"
#include "GHPlatform/GHDebugMessage.h"

GHTextureBMPSaver::GHTextureBMPSaver(const GHFileOpener& fileOpener)
	: mFileOpener(fileOpener)
{

}

namespace
{
	//BitmapHeader and BitmapInfoHeader from https://en.wikipedia.org/wiki/BMP_file_format
	// Using uint8_t to eliminate padding and also help ensure all integer values are little-endian (per spec) 
	struct BitmapHeader
	{
		uint8_t BM[2]{ 'B', 'M' };
		uint8_t mFileSize[4]{ 0 };
		uint8_t mUnused[4]{ 0 };
		uint8_t mOffset[4]{ 0 };
	};
	static_assert(sizeof(BitmapHeader) == 14, "Malformed BitmapHeader");

	struct BitmapInfoHeader
	{
		uint8_t mBIHSize[4]{ 0, 0, 0, 0 };
		uint8_t mBMWidth[4]{ 0 };
		uint8_t mBMHeight[4]{ 0 };
		uint8_t mNumColorPlanes[2]{ 1, 0 }; //Default value: always 1
		uint8_t mBitsPerPixel[2]{ 0 };

		uint8_t mCompression[4]{ 0 };
		uint8_t mImageSize[4]{ 0 };
		uint8_t mHRes[4]{ 0 };
		uint8_t mVRes[4]{ 0 };
		uint8_t mNumColors[4]{ 0 };
		uint8_t mNumImportantColors[4]{ 0 };
	};
	static_assert(sizeof(BitmapInfoHeader) == 40, "Malformed BitmapInfoHeader");

	struct BitmapV4Header //based on https://docs.microsoft.com/en-us/windows/win32/api/wingdi/ns-wingdi-bitmapv4header
	{
		uint8_t mRedMask[4]{ 0 };
		uint8_t mGreenMask[4]{ 0 };
		uint8_t mBlueMask[4]{ 0 };
		uint8_t mAlphaMask[4]{ 0 };
		uint8_t mCSType[4]{ 0 };
		uint8_t mEndpoints[36]{ 0 };
		uint8_t mGammaRed[4]{ 0 };
		uint8_t mGammaGreen[4]{ 0 };
		uint8_t mGammaBlue[4]{ 0 };
	};
	static_assert(sizeof(BitmapV4Header) + sizeof(BitmapInfoHeader) == 108, "Malformed BitmapV4Header");


	void setValueLE(uint32_t input, uint8_t(&output)[4])
	{
		output[0] = (uint8_t)input;
		output[1] = (uint8_t)(input >> 8);
		output[2] = (uint8_t)(input >> 16);
		output[3] = (uint8_t)(input >> 24);
	}

	void setValueLE(uint16_t input, uint8_t(&output)[2])
	{
		output[0] = (uint8_t)input;
		output[1] = (uint8_t)(input >> 8);
	}


	inline bool supported32Format(GHTextureFormat::Enum format)
	{
		return format == GHTextureFormat::TF_RGBA8 || format == GHTextureFormat::TF_BGRA8;
	}

	inline bool supported24Format(GHTextureFormat::Enum format)
	{
		return format == GHTextureFormat::TF_RGB8 || format == GHTextureFormat::TF_BGR8;
	}

	inline bool suportedFormat(GHTextureFormat::Enum format)
	{
		return supported32Format(format) || supported24Format(format);
	}

	inline bool supportedBGRFormat(GHTextureFormat::Enum format)
	{
		return format == GHTextureFormat::TF_BGR8 || format == GHTextureFormat::TF_BGRA8;
	}
	
	inline bool supportedRGBFormat(GHTextureFormat::Enum format)
	{
		return format == GHTextureFormat::TF_RGB8 || format == GHTextureFormat::TF_RGBA8;
	}

	inline uint8_t getRedOffset(GHTextureFormat::Enum format)
	{
		if (supportedBGRFormat(format))
		{
			return 2;
		}
		else //assume RGB
		{
			return 0;
		}
	}

	inline uint8_t getBlueOffset(GHTextureFormat::Enum format)
	{
		if (supportedBGRFormat(format))
		{
			return 0;
		}
		else //assume RGB
		{
			return 2;
		}
	}

	inline uint8_t getGreenOffset(GHTextureFormat::Enum format)
	{
		//All our supported formats have green in the same position
		return 1;
	}

	inline uint32_t getDepth(GHTextureFormat::Enum format)
	{
		if (supported32Format(format))
		{
			return 4;
		}
		else
		{
			return 3;
		}
	}
}

bool GHTextureBMPSaver::saveTexture(GHTexture& texture, const char* filename, GHTextureFormat::Enum srcFormat, GHTextureFormat::Enum dstFormat) const
{
	if (!suportedFormat(srcFormat) || !suportedFormat(dstFormat)) {
		GHDebugMessage::outputString("GHTextureBMPSaver unsupported format.");
		return false;
	}

	bool writeAlpha = supported32Format(dstFormat);
	bool useBitfields = writeAlpha || supportedBGRFormat(dstFormat);

	unsigned int width, height, srcDepth;
	if (!texture.getDimensions(width, height, srcDepth)) {
		GHDebugMessage::outputString("Error in GHTextureBMPSaver: unable to get texture dimensions");
		return false;
	}

	void* pixels;
	unsigned int pitch;
	if (!texture.lockSurface(&pixels, pitch)) {
		GHDebugMessage::outputString("Error in GHTextureBMPSaver: unable to lock texture");
		return false;
	}


	GHFile* file = mFileOpener.openFile(filename, GHFile::FileType::FT_BINARY, GHFile::FileMode::FM_WRITEONLY);
	if (!file) {
		GHDebugMessage::outputString("Error in GHTextureBMPSaver: unable to open file %s", filename);
		texture.unlockSurface();
		return false;
	}

	uint32_t dstDepth = getDepth(dstFormat);

	BitmapHeader bitmapHeader;
	BitmapInfoHeader bitmapInfoHeader;

	uint32_t combinedHeaderSize = sizeof(BitmapHeader) + sizeof(BitmapInfoHeader) + (useBitfields ? sizeof(BitmapV4Header) : 0);
	uint32_t outputTextureDataSize = width * height * dstDepth;
	outputTextureDataSize += outputTextureDataSize % 4;

	setValueLE(combinedHeaderSize, bitmapHeader.mOffset);
	setValueLE(combinedHeaderSize + outputTextureDataSize, bitmapHeader.mFileSize);

	setValueLE(combinedHeaderSize - sizeof(BitmapHeader), bitmapInfoHeader.mBIHSize);
	setValueLE(width, bitmapInfoHeader.mBMWidth);
	setValueLE(height, bitmapInfoHeader.mBMHeight);
	setValueLE(uint16_t(dstDepth*8), bitmapInfoHeader.mBitsPerPixel);
	constexpr uint32_t BI_BITFIELDS = 3;
	setValueLE(writeAlpha ? 3 : 0, bitmapInfoHeader.mCompression); //BI_BITFIELDS for alpha, BI_RGB wthout alpha
	setValueLE(outputTextureDataSize, bitmapInfoHeader.mImageSize);

	file->writeBuffer(&bitmapHeader, sizeof(bitmapHeader));
	file->writeBuffer(&bitmapInfoHeader, sizeof(bitmapInfoHeader));

	if (useBitfields)
	{
		BitmapV4Header v4Header;
		setValueLE(uint32_t(0xFF) << (getRedOffset(dstFormat) * 8), v4Header.mRedMask);
		setValueLE(uint32_t(0xFF) << (getGreenOffset(dstFormat) * 8), v4Header.mGreenMask);
		setValueLE(uint32_t(0xFF) << (getBlueOffset(dstFormat) * 8), v4Header.mBlueMask);
		setValueLE(uint32_t(writeAlpha ? 0xFF000000 : 0), v4Header.mAlphaMask);
		file->writeBuffer(&v4Header, sizeof(v4Header));
	}

	// The size of each row is rounded up to a multiple of 4 bytes (a 32-bit DWORD) by padding.
	uint8_t* padding[3] = { 0 };
	uint8_t numPaddingBytes = (4 - ((width*dstDepth) % 4)) % 4;

	//Can copy rows wholesale, no need to swizzle values. Do invert Y thought.
	if (srcDepth == dstDepth && srcFormat == dstFormat)
	{
		const uint8_t* imgData = (const uint8_t*)pixels;
		for (int h = height - 1; h >= 0; --h)
		{
			file->writeBuffer(imgData + (h * pitch), width * dstDepth);
			if (numPaddingBytes) {
				file->writeBuffer(padding, numPaddingBytes);
			}
		}
	}
	else
	{
		uint32_t srcRedOffset = getRedOffset(srcFormat);
		uint32_t srcGreenOffset = getGreenOffset(srcFormat);
		uint32_t srcBlueOffset = getBlueOffset(srcFormat);

		uint32_t dstRedOffset = getRedOffset(dstFormat);
		uint32_t dstGreenOffset = getGreenOffset(dstFormat);
		uint32_t dstBlueOffset = getBlueOffset(dstFormat);

		bool srcAlpha = supported32Format(srcFormat);

		const uint8_t* imgData = (const uint8_t*)pixels;
		for (int h = height - 1; h >= 0; --h)
		{
			const uint8_t* srcRowStart = imgData + (h * pitch);

			for (int w = 0; w < (int)width; ++w)
			{
				const uint8_t* srcPixel = srcRowStart + (w * srcDepth);

				uint8_t dstPixel[4];
				dstPixel[dstRedOffset] = srcPixel[srcRedOffset];
				dstPixel[dstGreenOffset] = srcPixel[srcGreenOffset];
				dstPixel[dstBlueOffset] = srcPixel[srcBlueOffset];
				if(writeAlpha)
				{
					dstPixel[3] = srcAlpha ? srcPixel[3] : 255;
				}
			
				file->writeBuffer(dstPixel, dstDepth);
			}

			if (numPaddingBytes) {
				file->writeBuffer(padding, numPaddingBytes);
			}
		}
	}



	file->closeFile();

	texture.unlockSurface();

	return true;
}
