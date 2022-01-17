#include "GHDXGIUtil.h"

namespace GHDXGIUtil
{
    GHDXGIFormat convertGHFormatToDXGI(GHTextureFormat::Enum ghFormat)
    {
        if (ghFormat == GHTextureFormat::TF_DXT1)
        {
            return GHDXGIFormat::DF_BC1_UNORM;
        }
        if (ghFormat == GHTextureFormat::TF_DXT5)
        {
            return GHDXGIFormat::DF_BC3_UNORM;
        }
        if (ghFormat == GHTextureFormat::TF_RGB8)
        {
            // there is no 24 bit d3d texture format.
            return GHDXGIFormat::DF_R8G8B8A8_UNORM;
        }
        if (ghFormat == GHTextureFormat::TF_RGBA8)
        {
            return GHDXGIFormat::DF_R8G8B8A8_UNORM;
        }
        return GHDXGIFormat::DF_R8G8B8A8_UNORM;
    }

    size_t bitsPerPixel(GHDXGIFormat fmt)
    {
        switch (fmt)
        {
        case GHDXGIFormat::DF_R32G32B32A32_TYPELESS:
        case GHDXGIFormat::DF_R32G32B32A32_FLOAT:
        case GHDXGIFormat::DF_R32G32B32A32_UINT:
        case GHDXGIFormat::DF_R32G32B32A32_SINT:
            return 128;

        case GHDXGIFormat::DF_R32G32B32_TYPELESS:
        case GHDXGIFormat::DF_R32G32B32_FLOAT:
        case GHDXGIFormat::DF_R32G32B32_UINT:
        case GHDXGIFormat::DF_R32G32B32_SINT:
            return 96;

        case GHDXGIFormat::DF_R16G16B16A16_TYPELESS:
        case GHDXGIFormat::DF_R16G16B16A16_FLOAT:
        case GHDXGIFormat::DF_R16G16B16A16_UNORM:
        case GHDXGIFormat::DF_R16G16B16A16_UINT:
        case GHDXGIFormat::DF_R16G16B16A16_SNORM:
        case GHDXGIFormat::DF_R16G16B16A16_SINT:
        case GHDXGIFormat::DF_R32G32_TYPELESS:
        case GHDXGIFormat::DF_R32G32_FLOAT:
        case GHDXGIFormat::DF_R32G32_UINT:
        case GHDXGIFormat::DF_R32G32_SINT:
        case GHDXGIFormat::DF_R32G8X24_TYPELESS:
        case GHDXGIFormat::DF_D32_FLOAT_S8X24_UINT:
        case GHDXGIFormat::DF_R32_FLOAT_X8X24_TYPELESS:
        case GHDXGIFormat::DF_X32_TYPELESS_G8X24_UINT:
            return 64;

        case GHDXGIFormat::DF_R10G10B10A2_TYPELESS:
        case GHDXGIFormat::DF_R10G10B10A2_UNORM:
        case GHDXGIFormat::DF_R10G10B10A2_UINT:
        case GHDXGIFormat::DF_R11G11B10_FLOAT:
        case GHDXGIFormat::DF_R8G8B8A8_TYPELESS:
        case GHDXGIFormat::DF_R8G8B8A8_UNORM:
        case GHDXGIFormat::DF_R8G8B8A8_UNORM_SRGB:
        case GHDXGIFormat::DF_R8G8B8A8_UINT:
        case GHDXGIFormat::DF_R8G8B8A8_SNORM:
        case GHDXGIFormat::DF_R8G8B8A8_SINT:
        case GHDXGIFormat::DF_R16G16_TYPELESS:
        case GHDXGIFormat::DF_R16G16_FLOAT:
        case GHDXGIFormat::DF_R16G16_UNORM:
        case GHDXGIFormat::DF_R16G16_UINT:
        case GHDXGIFormat::DF_R16G16_SNORM:
        case GHDXGIFormat::DF_R16G16_SINT:
        case GHDXGIFormat::DF_R32_TYPELESS:
        case GHDXGIFormat::DF_D32_FLOAT:
        case GHDXGIFormat::DF_R32_FLOAT:
        case GHDXGIFormat::DF_R32_UINT:
        case GHDXGIFormat::DF_R32_SINT:
        case GHDXGIFormat::DF_R24G8_TYPELESS:
        case GHDXGIFormat::DF_D24_UNORM_S8_UINT:
        case GHDXGIFormat::DF_R24_UNORM_X8_TYPELESS:
        case GHDXGIFormat::DF_X24_TYPELESS_G8_UINT:
        case GHDXGIFormat::DF_R9G9B9E5_SHAREDEXP:
        case GHDXGIFormat::DF_R8G8_B8G8_UNORM:
        case GHDXGIFormat::DF_G8R8_G8B8_UNORM:
        case GHDXGIFormat::DF_B8G8R8A8_UNORM:
        case GHDXGIFormat::DF_B8G8R8X8_UNORM:
        case GHDXGIFormat::DF_R10G10B10_XR_BIAS_A2_UNORM:
        case GHDXGIFormat::DF_B8G8R8A8_TYPELESS:
        case GHDXGIFormat::DF_B8G8R8A8_UNORM_SRGB:
        case GHDXGIFormat::DF_B8G8R8X8_TYPELESS:
        case GHDXGIFormat::DF_B8G8R8X8_UNORM_SRGB:
            return 32;

        case GHDXGIFormat::DF_R8G8_TYPELESS:
        case GHDXGIFormat::DF_R8G8_UNORM:
        case GHDXGIFormat::DF_R8G8_UINT:
        case GHDXGIFormat::DF_R8G8_SNORM:
        case GHDXGIFormat::DF_R8G8_SINT:
        case GHDXGIFormat::DF_R16_TYPELESS:
        case GHDXGIFormat::DF_R16_FLOAT:
        case GHDXGIFormat::DF_D16_UNORM:
        case GHDXGIFormat::DF_R16_UNORM:
        case GHDXGIFormat::DF_R16_UINT:
        case GHDXGIFormat::DF_R16_SNORM:
        case GHDXGIFormat::DF_R16_SINT:
        case GHDXGIFormat::DF_B5G6R5_UNORM:
        case GHDXGIFormat::DF_B5G5R5A1_UNORM:

#ifdef DXGI_1_2_FORMATS
        case GHDXGIFormat::DF_B4G4R4A4_UNORM:
#endif
            return 16;

        case GHDXGIFormat::DF_R8_TYPELESS:
        case GHDXGIFormat::DF_R8_UNORM:
        case GHDXGIFormat::DF_R8_UINT:
        case GHDXGIFormat::DF_R8_SNORM:
        case GHDXGIFormat::DF_R8_SINT:
        case GHDXGIFormat::DF_A8_UNORM:
            return 8;

        case GHDXGIFormat::DF_R1_UNORM:
            return 1;

        case GHDXGIFormat::DF_BC1_TYPELESS:
        case GHDXGIFormat::DF_BC1_UNORM:
        case GHDXGIFormat::DF_BC1_UNORM_SRGB:
        case GHDXGIFormat::DF_BC4_TYPELESS:
        case GHDXGIFormat::DF_BC4_UNORM:
        case GHDXGIFormat::DF_BC4_SNORM:
            return 4;

        case GHDXGIFormat::DF_BC2_TYPELESS:
        case GHDXGIFormat::DF_BC2_UNORM:
        case GHDXGIFormat::DF_BC2_UNORM_SRGB:
        case GHDXGIFormat::DF_BC3_TYPELESS:
        case GHDXGIFormat::DF_BC3_UNORM:
        case GHDXGIFormat::DF_BC3_UNORM_SRGB:
        case GHDXGIFormat::DF_BC5_TYPELESS:
        case GHDXGIFormat::DF_BC5_UNORM:
        case GHDXGIFormat::DF_BC5_SNORM:
        case GHDXGIFormat::DF_BC6H_TYPELESS:
        case GHDXGIFormat::DF_BC6H_UF16:
        case GHDXGIFormat::DF_BC6H_SF16:
        case GHDXGIFormat::DF_BC7_TYPELESS:
        case GHDXGIFormat::DF_BC7_UNORM:
        case GHDXGIFormat::DF_BC7_UNORM_SRGB:
            return 8;

        default:
            return 0;
        }
    }

    void getSurfaceInfo(_In_ size_t width,
        _In_ size_t height,
        _In_ GHDXGIFormat fmt,
        _Out_opt_ size_t* outNumBytes,
        _Out_opt_ size_t* outRowBytes,
        _Out_opt_ size_t* outNumRows)
    {
        size_t numBytes = 0;
        size_t rowBytes = 0;
        size_t numRows = 0;

        bool bc = false;
        bool packed = false;
        size_t bcnumBytesPerBlock = 0;
        switch (fmt)
        {
        case GHDXGIFormat::DF_BC1_TYPELESS:
        case GHDXGIFormat::DF_BC1_UNORM:
        case GHDXGIFormat::DF_BC1_UNORM_SRGB:
        case GHDXGIFormat::DF_BC4_TYPELESS:
        case GHDXGIFormat::DF_BC4_UNORM:
        case GHDXGIFormat::DF_BC4_SNORM:
            bc = true;
            bcnumBytesPerBlock = 8;
            break;

        case GHDXGIFormat::DF_BC2_TYPELESS:
        case GHDXGIFormat::DF_BC2_UNORM:
        case GHDXGIFormat::DF_BC2_UNORM_SRGB:
        case GHDXGIFormat::DF_BC3_TYPELESS:
        case GHDXGIFormat::DF_BC3_UNORM:
        case GHDXGIFormat::DF_BC3_UNORM_SRGB:
        case GHDXGIFormat::DF_BC5_TYPELESS:
        case GHDXGIFormat::DF_BC5_UNORM:
        case GHDXGIFormat::DF_BC5_SNORM:
        case GHDXGIFormat::DF_BC6H_TYPELESS:
        case GHDXGIFormat::DF_BC6H_UF16:
        case GHDXGIFormat::DF_BC6H_SF16:
        case GHDXGIFormat::DF_BC7_TYPELESS:
        case GHDXGIFormat::DF_BC7_UNORM:
        case GHDXGIFormat::DF_BC7_UNORM_SRGB:
            bc = true;
            bcnumBytesPerBlock = 16;
            break;

        case GHDXGIFormat::DF_R8G8_B8G8_UNORM:
        case GHDXGIFormat::DF_G8R8_G8B8_UNORM:
            packed = true;
            break;
        }

        if (bc)
        {
            size_t numBlocksWide = 0;
            if (width > 0)
            {
                numBlocksWide = std::max<size_t>(1, (width + 3) / 4);
            }
            size_t numBlocksHigh = 0;
            if (height > 0)
            {
                numBlocksHigh = std::max<size_t>(1, (height + 3) / 4);
            }
            rowBytes = numBlocksWide * bcnumBytesPerBlock;
            numRows = numBlocksHigh;
        }
        else if (packed)
        {
            rowBytes = ((width + 1) >> 1) * 4;
            numRows = height;
        }
        else
        {
            size_t bpp = GHDXGIUtil::bitsPerPixel(fmt);
            rowBytes = (width * bpp + 7) / 8; // round up to nearest byte
            numRows = height;
        }

        numBytes = rowBytes * numRows;
        if (outNumBytes)
        {
            *outNumBytes = numBytes;
        }
        if (outRowBytes)
        {
            *outRowBytes = rowBytes;
        }
        if (outNumRows)
        {
            *outNumRows = numRows;
        }
    }

    bool isDepthStencil(GHDXGIFormat fmt)
    {
        switch (fmt)
        {
        case GHDXGIFormat::DF_R32G8X24_TYPELESS:
        case GHDXGIFormat::DF_D32_FLOAT_S8X24_UINT:
        case GHDXGIFormat::DF_R32_FLOAT_X8X24_TYPELESS:
        case GHDXGIFormat::DF_X32_TYPELESS_G8X24_UINT:
        case GHDXGIFormat::DF_D32_FLOAT:
        case GHDXGIFormat::DF_R24G8_TYPELESS:
        case GHDXGIFormat::DF_D24_UNORM_S8_UINT:
        case GHDXGIFormat::DF_R24_UNORM_X8_TYPELESS:
        case GHDXGIFormat::DF_X24_TYPELESS_G8_UINT:
        case GHDXGIFormat::DF_D16_UNORM:
            return true;

        default:
            return false;
        }
    }
};

