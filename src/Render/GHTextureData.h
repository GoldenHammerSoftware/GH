#pragma once

#include "GHTextureFormat.h"
#include "GHTextureChannelType.h"
#include "GHTextureType.h"
#include <vector>

// Platform and format independent texture data description.
// Todo: Evaluate whether we want to encapsulate the data structures inside.
struct GHTextureData
{
	~GHTextureData(void)
	{
		delete mDataSource;
	}

	struct MipData
	{
		size_t mWidth{ 0 };
		size_t mHeight{ 0 };
		void* mData{ 0 };
		size_t mDataSize{ 0 };
		size_t mRowPitch{ 0 };
	};

	GHTextureFormat::Enum mTextureFormat{ GHTextureFormat::TF_UNKNOWN };
	GHTextureChannelType::Enum mChannelType{ GHTextureChannelType::TC_UNKNOWN };
	GHTextureType::Enum mTextureType{ GHTextureType::TT_2D };
	size_t mDepth{ 4 };
	uint8_t mNumSlices;
	std::vector<MipData> mMipLevels;

	// The source data block.
	// We own this and delete it.
	int8_t* mDataSource{ 0 };
};
