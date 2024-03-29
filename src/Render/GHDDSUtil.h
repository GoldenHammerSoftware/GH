#pragma once
// This is the api-independent code for loading a DDS file to support dx11 and dx12.
// much of this is from microsoft.
// http://go.microsoft.com/fwlink/?LinkId=248926
// http://go.microsoft.com/fwlink/?LinkId=248929

#include <algorithm>
#include <memory>
#include "Render/GHDXGIFormat.h"
#include "Render/GHTextureType.h"

namespace GHDDSUtil
{

//--------------------------------------------------------------------------------------
// DDS file structure definitions
//
// See DDS.h in the 'Texconv' sample and the 'DirectXTex' library
//--------------------------------------------------------------------------------------
#pragma pack(push,1)

    struct DDS_PIXELFORMAT
    {
        uint32_t    size;
        uint32_t    flags;
        uint32_t    fourCC;
        uint32_t    RGBBitCount;
        uint32_t    RBitMask;
        uint32_t    GBitMask;
        uint32_t    BBitMask;
        uint32_t    ABitMask;
    };

    typedef struct
    {
        uint32_t        size;
        uint32_t        flags;
        uint32_t        height;
        uint32_t        width;
        uint32_t        pitchOrLinearSize;
        uint32_t        depth; // only if DDS_HEADER_FLAGS_VOLUME is set in flags
        uint32_t        mipMapCount;
        uint32_t        reserved1[11];
        DDS_PIXELFORMAT ddspf;
        uint32_t        caps;
        uint32_t        caps2;
        uint32_t        caps3;
        uint32_t        caps4;
        uint32_t        reserved2;
    } DDS_HEADER;

    typedef struct
    {
        GHDXGIFormat    dxgiFormat;
        uint32_t        resourceDimension;
        uint32_t        miscFlag; // see D3D11_RESOURCE_MISC_FLAG
        uint32_t        arraySize;
        uint32_t        reserved;
    } DDS_HEADER_DXT10;

    struct DDSDesc
    {
        size_t width;
        size_t height;
        size_t depth;
        size_t arraySize;
        size_t mipCount;
        GHDXGIFormat format;
        uint32_t resDim;
        bool isCubeMap;
        size_t skipMip;
        // dims of top size used
        size_t twidth;
        size_t theight;
        size_t tdepth;
    };

    // must match D3D11_RESOURCE_DIMENSION and D3D12_RESOURCE_DIMENSION
    typedef enum DDS_RESOURCE_DIMENSION
    {
        DDS_RESOURCE_DIMENSION_UNKNOWN = 0,
        DDS_RESOURCE_DIMENSION_BUFFER = 1,
        DDS_RESOURCE_DIMENSION_TEXTURE1D = 2,
        DDS_RESOURCE_DIMENSION_TEXTURE2D = 3,
        DDS_RESOURCE_DIMENSION_TEXTURE3D = 4
    } 	DDS_RESOURCE_DIMENSION;

    GHTextureType::Enum getGHTextureType(DDS_RESOURCE_DIMENSION dim);
    DDS_RESOURCE_DIMENSION getDXTextureType(GHTextureType::Enum type);

    // matches D3D11_SUBRESOURCE_DATA to avoid dx11/12 include issues.
    typedef struct SubresourceData
    {
        const void* pSysMem;
        uint32_t SysMemPitch;
        uint32_t SysMemSlicePitch; // for 3d only
    } 	SubresourceData;

#pragma pack(pop)

    bool LoadTextureDataFromMemory(
        uint8_t* data, size_t dataSize,
        DDS_HEADER** header,
        uint8_t** bitData,
        size_t* bitSize
    );

    bool FillInitData(DDSDesc& desc,
        _In_ size_t maxsize,
        _In_ size_t bitSize,
        _In_bytecount_(bitSize) const uint8_t* bitData,
        _Out_cap_(mipCount* arraySize) SubresourceData* initData);

    bool parseHeaderMemory(_In_bytecount_(ddsDataSize) const uint8_t* ddsData,
        _In_ size_t ddsDataSize,
        const DDS_HEADER*& outHeader,
        ptrdiff_t& outDataOffset);

    bool validateAndParseHeader(const DDS_HEADER& header, DDSDesc& desc);

};
