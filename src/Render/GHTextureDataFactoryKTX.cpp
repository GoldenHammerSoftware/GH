#include "GHTextureDataFactoryKTX.h"
#include <stdint.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHTextureData.h"
#include "GHTextureTypeUtil.h"

// https://www.khronos.org/opengles/sdk/tools/KTX/file_format_spec/
struct KTXHeader
{
	int8_t identifier[12];
	uint32_t endianness;
	uint32_t glType;
	uint32_t glTypeSize;
	uint32_t glFormat;
	uint32_t glInternalFormat;
	uint32_t glBaseInternalFormat;
	uint32_t pixelWidth;
	uint32_t pixelHeight;
	uint32_t pixelDepth;
	uint32_t numberOfArrayElements;
	uint32_t numberOfFaces;
	uint32_t numberOfMipmapLevels;
	uint32_t bytesOfKeyValueData;
};

GHTextureData* GHTextureDataFactoryKTX::createFromMemory(void* data, size_t dataSize) const
{
	if (dataSize < sizeof(KTXHeader))
	{
		return 0;
	}
	KTXHeader* header = (KTXHeader*)data;

	int8_t* keyValueDataStart = (int8_t*)data + sizeof(KTXHeader);
	int8_t* mipStart = keyValueDataStart + header->bytesOfKeyValueData;

	// verify ktx
	int8_t ktxIdentifier[12] = {
		//0xAB, 0x4B, 0x54, 0x58, 0x20, 0x31, 0x31, 0xBB, 0x0D, 0x0A, 0x1A, 0x0A
		(int8_t)(0xAB), (int8_t)('K'), (int8_t)('T'), (int8_t)('X'), (int8_t)(' '), (int8_t)('1'), (int8_t)('1'), (int8_t)(0xBB), (int8_t)('\r'), (int8_t)('\n'), (int8_t)(0x1A), (int8_t)('\n')
	};
	for (size_t i = 0; i < 12; ++i)
	{
		if (header->identifier[i] != ktxIdentifier[i])
		{
			GHDebugMessage::outputString("invalid ktx file?");
			return 0;
		}
	}

	// todo: header->endianness

	if (header->numberOfArrayElements > 0)
	{
		// "numberOfArrayElements specifies the number of array elements. If the texture is not an array texture, numberOfArrayElements must equal 0."
		GHDebugMessage::outputString("array ktx unsupported");
		return 0;
	}
	if (header->numberOfFaces != 1)
	{
		// "numberOfFaces specifies the number of cubemap faces. For cubemaps and cubemap arrays this should be 6. For non cubemaps this should be 1"
		GHDebugMessage::outputString("cubemap ktx unsupported");
		return 0;
	}

	// header->glFormat is RGB/RGBA/etc and is used for the format parameter in glTexture2d.  0 for compressed.
	// header->glInternalFormat is the typed format used for compressed. PVR/RGBA8/etc.
	// header->glBaseInternalFormat is the untyped format.  Might need to pass this to gles2 instead of the typed version for uncompressed.
	// todo: calc gh texture format from header->glFormat/glInternalFormat/glBaseInternalFormat
	GHTextureFormat::Enum ghTextureFormat = GHTextureTypeUtil::getGHFormat(header->glInternalFormat);
	if (ghTextureFormat == GHTextureFormat::TF_UNKNOWN)
	{
		GHDebugMessage::outputString("Encountered unknown texture type in KTX file");
		return 0;
	}

	GHTextureData* ret = new GHTextureData();
	ret->mDataSource = (int8_t*)data;
	ret->mDepth = header->pixelDepth; 

	// glbyte etc.
	ret->mChannelType = GHTextureTypeUtil::getGHChannelType(header->glType);
	ret->mTextureFormat = ghTextureFormat;
	uint32_t width = header->pixelWidth;
	uint32_t height = header->pixelHeight;

	ret->mNumMips = header->numberOfMipmapLevels;
	ret->mMipLevels.resize(header->numberOfMipmapLevels);
	for (size_t mipLevel = 0; mipLevel < header->numberOfMipmapLevels; ++mipLevel)
	{
		uint32_t mipLevelSize = *(uint32_t*)mipStart;
		int8_t* mipDataStart = mipStart + sizeof(uint32_t);

		ret->mMipLevels[mipLevel].mData = mipDataStart;
		ret->mMipLevels[mipLevel].mDataSize = mipLevelSize;
		ret->mMipLevels[mipLevel].mHeight = height;
		ret->mMipLevels[mipLevel].mWidth = width;
		ret->mMipLevels[mipLevel].mRowPitch = ret->mMipLevels[mipLevel].mDataSize / ret->mMipLevels[mipLevel].mHeight;

		mipStart = mipDataStart + mipLevelSize;

		width >>= 1;
		height >>= 1;
		if (width < 1) { width = 1; }
		if (height < 1) { height = 1; }
	}

	return ret;
}
