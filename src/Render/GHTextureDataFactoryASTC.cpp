#include "GHTextureDataFactoryASTC.h"
#include <cstdint>
#include <assert.h>
#include "GHPlatform/GHDebugMessage.h"
#include "GHTextureFormat.h"
#include "GHTextureData.h"
#include "GHMath/GHPoint.h"

struct ASTCHeader
{
	uint8_t		magic[4];
	uint8_t		blockDim_x;
	uint8_t		blockDim_y;
	uint8_t		blockDim_z;
	uint8_t		xsize[3];
	uint8_t		ysize[3];
	uint8_t		zsize[3];
};

static GHTextureFormat::Enum getGHTextureFormat(const ASTCHeader& header)
{
	if (header.blockDim_z != 1)
	{
		assert(header.blockDim_z == 1);
		GHDebugMessage::outputString("Only 2D ASTC textures are supported");
		return GHTextureFormat::TF_UNKNOWN;
	}

	if (header.blockDim_x == 4)
	{
		if (header.blockDim_y == 4)
		{
			return GHTextureFormat::TF_ASTC_4x4;
		}
	}
	else if (header.blockDim_x == 5)
	{
		if (header.blockDim_y == 4)
		{
			return GHTextureFormat::TF_ASTC_5x4;
		}
		else if (header.blockDim_y == 5)
		{
			return GHTextureFormat::TF_ASTC_5x5;
		}
	}
	else if (header.blockDim_x == 6)
	{
		if (header.blockDim_y == 5)
		{
			return GHTextureFormat::TF_ASTC_6x5;
		}
		else if (header.blockDim_y == 6)
		{
			return GHTextureFormat::TF_ASTC_6x6;
		}
	}
	else if (header.blockDim_x == 8)
	{
		if (header.blockDim_y == 5)
		{
			return GHTextureFormat::TF_ASTC_8x5;
		}
		else if (header.blockDim_y == 6)
		{
			return GHTextureFormat::TF_ASTC_8x6;
		}
		else if (header.blockDim_y == 8)
		{
			return GHTextureFormat::TF_ASTC_8x8;
		}
	}
	else if (header.blockDim_x == 10)
	{
		if (header.blockDim_y == 5)
		{
			return GHTextureFormat::TF_ASTC_10x5;
		}
		else if (header.blockDim_y == 6)
		{
			return GHTextureFormat::TF_ASTC_10x6;
		}
		else if (header.blockDim_y == 8)
		{
			return GHTextureFormat::TF_ASTC_10x8;
		}
		else if (header.blockDim_y == 10)
		{
			return GHTextureFormat::TF_ASTC_10x10;
		}
	}
	else if (header.blockDim_x == 12)
	{
		if (header.blockDim_y == 10)
		{
			return GHTextureFormat::TF_ASTC_12x10;
		}
		else if (header.blockDim_y == 12)
		{
			return GHTextureFormat::TF_ASTC_12x12;
		}
	}
	return GHTextureFormat::TF_UNKNOWN;
}

static void getBlockDimFromTextureFormat(GHTextureFormat::Enum textureFormat, GHPoint3i& outBlockDim)
{
	switch (textureFormat)
	{
	case GHTextureFormat::TF_ASTC_4x4:		outBlockDim.setCoords(4,4,1);			break;
	case GHTextureFormat::TF_ASTC_5x4:		outBlockDim.setCoords(5,4,1);			break;
	case GHTextureFormat::TF_ASTC_5x5:		outBlockDim.setCoords(5,5,1);			break;
	case GHTextureFormat::TF_ASTC_6x5:		outBlockDim.setCoords(6,5,1);			break;
	case GHTextureFormat::TF_ASTC_6x6:		outBlockDim.setCoords(6,6,1);			break;
	case GHTextureFormat::TF_ASTC_8x5:		outBlockDim.setCoords(8,5,1);			break;
	case GHTextureFormat::TF_ASTC_8x6:		outBlockDim.setCoords(8,6,1);			break;
	case GHTextureFormat::TF_ASTC_8x8:		outBlockDim.setCoords(8,8,1);			break;
	case GHTextureFormat::TF_ASTC_10x5:		outBlockDim.setCoords(10,5,1);			break;
	case GHTextureFormat::TF_ASTC_10x6:		outBlockDim.setCoords(10,6,1);			break;
	case GHTextureFormat::TF_ASTC_10x8:		outBlockDim.setCoords(10,8,1);			break;
	case GHTextureFormat::TF_ASTC_10x10:	outBlockDim.setCoords(10,10,1);			break;
	case GHTextureFormat::TF_ASTC_12x10:	outBlockDim.setCoords(12,10,1);			break;
	case GHTextureFormat::TF_ASTC_12x12:	outBlockDim.setCoords(12,12,1);			break;
	case GHTextureFormat::TF_ASTC_3x3x3:	outBlockDim.setCoords(3,3,3);			break;
	case GHTextureFormat::TF_ASTC_4x3x3:	outBlockDim.setCoords(4,3,3);			break;
	case GHTextureFormat::TF_ASTC_4x4x3:	outBlockDim.setCoords(4,4,3);			break;
	case GHTextureFormat::TF_ASTC_4x4x4:	outBlockDim.setCoords(4,4,4);			break;
	case GHTextureFormat::TF_ASTC_5x4x4:	outBlockDim.setCoords(5,4,4);			break;
	case GHTextureFormat::TF_ASTC_5x5x4:	outBlockDim.setCoords(5,5,4);			break;
	case GHTextureFormat::TF_ASTC_5x5x5:	outBlockDim.setCoords(5,5,5);			break;
	case GHTextureFormat::TF_ASTC_6x5x5:	outBlockDim.setCoords(6,5,5);			break;
	case GHTextureFormat::TF_ASTC_6x6x5:	outBlockDim.setCoords(6,6,5);			break;
	case GHTextureFormat::TF_ASTC_6x6x6:	outBlockDim.setCoords(6,6,6);			break;
	default:
		assert(false && "invalid ASTC texture format (note: 3d texture support hasn't been added yet)");
	}
}

static uint32_t getTextureBufferSize(const GHPoint3i& blockDim, const GHPoint2i textureDim)
{
	// todo: support 3d textures.
	uint32_t xBlocks = (textureDim[0] + blockDim[0] - 1) / blockDim[0];
	uint32_t yBlocks = (textureDim[1] + blockDim[1] - 1) / blockDim[1];
	return xBlocks * yBlocks * 16;
}

static GHTextureData* createAstcTextureDataFromMemoryCommon(const GHPoint3i& blockDim, const GHPoint2i textureDim, GHTextureFormat::Enum textureFormat, void* dataSource, void* textureStart, size_t textureSize, size_t mipLevels)
{
	//The main texture counts as a mip level
	if (mipLevels == 0) {
		mipLevels = 1;
	}

	GHTextureData* ret = new GHTextureData();
	ret->mDataSource = (int8_t*)dataSource;
	ret->mDepth = 4; // not correct but doesn't matter?
	ret->mChannelType = GHTextureChannelType::TC_UBYTE; // not really correct but doesn't matter?
	ret->mTextureFormat = textureFormat;

	void* mipData = textureStart;
	ret->mNumMips = (uint8_t)mipLevels;
	ret->mMipLevels.resize(mipLevels);
	GHPoint2i mipDim = textureDim;
	for (size_t i = 0; i < mipLevels; ++i)
	{
		uint32_t mipSize = getTextureBufferSize(blockDim, mipDim);
		ret->mMipLevels[i].mData = mipData;
		ret->mMipLevels[i].mDataSize = mipSize;
		ret->mMipLevels[i].mHeight = mipDim[1];
		ret->mMipLevels[i].mWidth = mipDim[0];
		ret->mMipLevels[i].mRowPitch = ret->mMipLevels[i].mDataSize / ret->mMipLevels[i].mHeight;

		mipData = ((uint8_t*)mipData) + mipSize;
		mipDim[0] >>= 1;
		mipDim[1] >>= 1;
		if (mipDim[0] < 1) { mipDim[0] = 1; }
		if (mipDim[1] < 1) { mipDim[1] = 1; }
	}
	return ret;
}



GHTextureData* GHTextureDataFactoryASTC::createFromMemory(void* data, size_t dataSize) const
{
	if (dataSize < sizeof(ASTCHeader))
	{
		return 0;
	}
	ASTCHeader* header = (ASTCHeader*)data;

	GHTextureFormat::Enum textureFormat = getGHTextureFormat(*header);
	if (textureFormat == GHTextureFormat::TF_UNKNOWN)
	{
		GHDebugMessage::outputString("Unhandled ASTC format");
		return 0;
	}

	void* textureStart = (int8_t*)data + sizeof(ASTCHeader);
	size_t textureSize = dataSize - sizeof(ASTCHeader);
	size_t mipLevels = 0; // todo: miplevels.  It seems .astc does not support them?
	int w = ((int)header->xsize[2] << 16) | ((int)header->xsize[1] << 8) | ((int)header->xsize[0]);
	int h = ((int)header->ysize[2] << 16) | ((int)header->ysize[1] << 8) | ((int)header->ysize[0]);
	return createAstcTextureDataFromMemoryCommon(GHPoint3i(header->blockDim_x, header->blockDim_y, header->blockDim_z), GHPoint2i(w, h), textureFormat, data, textureStart, textureSize, mipLevels);
}

GHTextureData* GHTextureDataFactoryASTC::createFromMemoryWithMips(void* data, size_t dataSize, GHTextureFormat::Enum textureFormat, unsigned int width, unsigned int height, unsigned int mipCount, bool headerExists)
{
	GHTextureData* ret = 0;
	if (headerExists)
	{
		ret = createFromMemory(data, dataSize);
	}
	else
	{
		GHPoint3i blockDim;
		getBlockDimFromTextureFormat(textureFormat, blockDim);
		ret = createAstcTextureDataFromMemoryCommon(blockDim, GHPoint2i(width, height), textureFormat, data, data, dataSize, mipCount);
	}

	assert(ret);
	assert(ret->mTextureFormat == textureFormat);
	assert(ret->mNumMips == mipCount || (mipCount == 0 && ret->mNumMips == 1));
	assert(ret->mMipLevels[0].mWidth == width);
	assert(ret->mMipLevels[0].mHeight == height);
	return ret;
}

