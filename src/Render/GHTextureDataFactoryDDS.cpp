#include "GHTextureDataFactoryDDS.h"
#include "GHDDSUtil.h"
#include "GHDXGIUtil.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHTextureData.h"
#include "GHMath/GHPoint.h"

struct ParsedDDS
{
    GHDDSUtil::DDS_HEADER* header{ 0 };
    uint8_t* bitData{ 0 };
    size_t bitSize{ 0 };
    GHDDSUtil::DDSDesc desc;
    GHDDSUtil::SubresourceData* initData{ 0 };
};

static bool parseDDSData(void* data, size_t dataSize, ParsedDDS& ddsInfo)
{
    bool ddsLoaded = GHDDSUtil::LoadTextureDataFromMemory((uint8_t*)data, dataSize, &ddsInfo.header, &ddsInfo.bitData, &ddsInfo.bitSize);
    if (!ddsLoaded)
    {
        GHDebugMessage::outputString("Failed to parse DDS memory");
        return false;
    }

    bool headerValidated = GHDDSUtil::validateAndParseHeader(*ddsInfo.header, ddsInfo.desc);
    if (!headerValidated)
    {
        GHDebugMessage::outputString("Failed to validate and parse DDS header");
        return false;
    }

    // Create the texture data
    ddsInfo.initData = (new GHDDSUtil::SubresourceData[ddsInfo.desc.mipCount * ddsInfo.desc.arraySize]);
    if (!ddsInfo.initData)
    {
        GHDebugMessage::outputString("DDS load OOM");
        return false;
    }

    bool parsedInitData = GHDDSUtil::FillInitData(ddsInfo.desc, 0, ddsInfo.bitSize, ddsInfo.bitData, ddsInfo.initData);
    if (!parsedInitData)
    {
        GHDebugMessage::outputString("Failed to parse DDS init data");
        return false;
    }
    return true;
}

GHTextureData* GHTextureDataFactoryDDS::createFromMemory(void* data, size_t dataSize) const
{
    ParsedDDS ddsInfo;
    bool parsed = parseDDSData(data, dataSize, ddsInfo);
    if (!parsed)
    {
        return 0;
    }

    // todo:
    return 0;

    // Convert from DDS data to a GHTextureData.
    GHTextureData* ret = new GHTextureData;
    ret->mDataSource = (int8_t*)data;
    ret->mTextureType = GHDDSUtil::getGHTextureType((GHDDSUtil::DDS_RESOURCE_DIMENSION)ddsInfo.desc.resDim);
    ret->mTextureFormat = GHDXGIUtil::convertDXGIFormatToGH(ddsInfo.desc.format);
    ret->mChannelType = GHTextureChannelType::TC_UNKNOWN;
    ret->mNumSlices = (uint8_t)ddsInfo.desc.arraySize; // todo? cubemap

    ret->mMipLevels.resize(ret->mNumSlices * ddsInfo.desc.mipCount);
    for (size_t slice = 0; slice < ret->mNumSlices; ++slice)
    {
        GHPoint2i size((int)ddsInfo.desc.width, (int)ddsInfo.desc.height);
        for (size_t mip = 0; mip < ddsInfo.desc.mipCount; ++mip)
        {
            const size_t arrIndex = (slice * ddsInfo.desc.mipCount) + mip;
            ret->mMipLevels[arrIndex].mData = (void*)ddsInfo.initData->pSysMem;

            // todo: width/height/depth/stride
        }
    }

    return ret;
}

/*
    struct MipData
    {
        size_t mWidth{ 0 };
        size_t mHeight{ 0 };
        void* mData{ 0 };
        size_t mDataSize{ 0 };
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
*/