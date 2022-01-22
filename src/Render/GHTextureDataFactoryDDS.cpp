#include "GHTextureDataFactoryDDS.h"
#include "GHDDSUtil.h"
#include "GHPlatform/GHDebugMessage.h"

GHTextureData* GHTextureDataFactoryDDS::createFromMemory(void* data, size_t dataSize) const
{
	GHDDSUtil::DDS_HEADER* header;
	uint8_t* bitData;
	size_t bitSize;
	bool ddsLoaded = GHDDSUtil::LoadTextureDataFromMemory((uint8_t*)data, dataSize, &header, &bitData, &bitSize);
	if (!ddsLoaded)
	{
		GHDebugMessage::outputString("Failed to parse DDS memory");
		return 0;
	}

    GHDDSUtil::DDSDesc desc;
    bool headerValidated = GHDDSUtil::validateAndParseHeader(*header, desc);
    if (!headerValidated)
    {
        GHDebugMessage::outputString("Failed to validate and parse DDS header");
        return 0;
    }

    // Create the texture data
    std::unique_ptr<GHDDSUtil::SubresourceData> initData(new GHDDSUtil::SubresourceData[desc.mipCount * desc.arraySize]);
    if (!initData)
    {
        GHDebugMessage::outputString("DDS load OOM");
        return 0;
    }

    bool parsedInitData = GHDDSUtil::FillInitData(desc, 0, bitSize, bitData, initData.get());
    if (!parsedInitData)
    {
        GHDebugMessage::outputString("Failed to parse DDS init data");
        return 0;
    }

    // Convert from DDS data to a GHTextureData.

	return 0;
}
