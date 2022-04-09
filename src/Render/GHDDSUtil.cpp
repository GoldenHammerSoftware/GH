#include "GHDDSUtil.h"
#include "GHDXGIUtil.h"
#include <assert.h>

namespace GHDDSUtil
{
    //--------------------------------------------------------------------------------------
    // Macros
    //--------------------------------------------------------------------------------------
    #ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                    ((uint32_t)(uint8_t)(ch0) | ((uint32_t)(uint8_t)(ch1) << 8) |       \
                    ((uint32_t)(uint8_t)(ch2) << 16) | ((uint32_t)(uint8_t)(ch3) << 24 ))
    #endif /* defined(MAKEFOURCC) */

    //--------------------------------------------------------------------------------------
    // DDS file structure definitions
    //
    // See DDS.h in the 'Texconv' sample and the 'DirectXTex' library
    //--------------------------------------------------------------------------------------
    #pragma pack(push,1)

    #define DDS_MAGIC 0x20534444 // "DDS "
    #define	DDS_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION	( 2048 )
    #define	DDS_REQ_TEXTURE1D_U_DIMENSION	( 16384 )
    #define	DDS_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION	( 2048 )
    #define	DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION	( 16384 )
    #define	DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION	( 2048 )
    #define	DDS_REQ_TEXTURECUBE_DIMENSION	( 16384 )
    #define	DDS_REQ_MIP_LEVELS	( 15 )
    #define DDS_RESOURCE_MISC_TEXTURECUBE 0x4L

    #define DDS_FOURCC      0x00000004  // DDPF_FOURCC
    #define DDS_RGB         0x00000040  // DDPF_RGB
    #define DDS_RGBA        0x00000041  // DDPF_RGB | DDPF_ALPHAPIXELS
    #define DDS_LUMINANCE   0x00020000  // DDPF_LUMINANCE
    #define DDS_LUMINANCEA  0x00020001  // DDPF_LUMINANCE | DDPF_ALPHAPIXELS
    #define DDS_ALPHA       0x00000002  // DDPF_ALPHA
    #define DDS_PAL8        0x00000020  // DDPF_PALETTEINDEXED8

    #define DDS_HEADER_FLAGS_TEXTURE        0x00001007  // DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH | DDSD_PIXELFORMAT 
    #define DDS_HEADER_FLAGS_MIPMAP         0x00020000  // DDSD_MIPMAPCOUNT
    #define DDS_HEADER_FLAGS_VOLUME         0x00800000  // DDSD_DEPTH
    #define DDS_HEADER_FLAGS_PITCH          0x00000008  // DDSD_PITCH
    #define DDS_HEADER_FLAGS_LINEARSIZE     0x00080000  // DDSD_LINEARSIZE

    #define DDS_HEIGHT 0x00000002 // DDSD_HEIGHT
    #define DDS_WIDTH  0x00000004 // DDSD_WIDTH

    #define DDS_SURFACE_FLAGS_TEXTURE 0x00001000 // DDSCAPS_TEXTURE
    #define DDS_SURFACE_FLAGS_MIPMAP  0x00400008 // DDSCAPS_COMPLEX | DDSCAPS_MIPMAP
    #define DDS_SURFACE_FLAGS_CUBEMAP 0x00000008 // DDSCAPS_COMPLEX

    #define DDS_CUBEMAP_POSITIVEX 0x00000600 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEX
    #define DDS_CUBEMAP_NEGATIVEX 0x00000a00 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEX
    #define DDS_CUBEMAP_POSITIVEY 0x00001200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEY
    #define DDS_CUBEMAP_NEGATIVEY 0x00002200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEY
    #define DDS_CUBEMAP_POSITIVEZ 0x00004200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_POSITIVEZ
    #define DDS_CUBEMAP_NEGATIVEZ 0x00008200 // DDSCAPS2_CUBEMAP | DDSCAPS2_CUBEMAP_NEGATIVEZ

    #define DDS_CUBEMAP_ALLFACES ( DDS_CUBEMAP_POSITIVEX | DDS_CUBEMAP_NEGATIVEX |\
                                   DDS_CUBEMAP_POSITIVEY | DDS_CUBEMAP_NEGATIVEY |\
                                   DDS_CUBEMAP_POSITIVEZ | DDS_CUBEMAP_NEGATIVEZ )

    #define DDS_CUBEMAP 0x00000200 // DDSCAPS2_CUBEMAP

    #define DDS_FLAGS_VOLUME 0x00200000 // DDSCAPS2_VOLUME

    #pragma pack(pop)

    //---------------------------------------------------------------------------------

    bool LoadTextureDataFromMemory(
        uint8_t* data, size_t dataSize,
        DDS_HEADER** header,
        uint8_t** bitData,
        size_t* bitSize
        )
    {
        // DDS files always start with the same magic number ("DDS ")
        uint32_t dwMagicNumber = *(const uint32_t*)(data);
        if (dwMagicNumber != DDS_MAGIC)
        {
            return false;
        }

        DDS_HEADER* hdr = reinterpret_cast<DDS_HEADER*>(data + sizeof(uint32_t));

        // Verify header to validate DDS file
        if (hdr->size != sizeof(DDS_HEADER) ||
            hdr->ddspf.size != sizeof(DDS_PIXELFORMAT))
        {
            return false;
        }

        // Check for DX10 extension
        bool bDXT10Header = false;
        if ((hdr->ddspf.flags & DDS_FOURCC) &&
            (MAKEFOURCC('D', 'X', '1', '0') == hdr->ddspf.fourCC))
        {
            // Must be long enough for both headers and magic value
            if (dataSize < (sizeof(DDS_HEADER) + sizeof(uint32_t) + sizeof(DDS_HEADER_DXT10)))
            {
                return false;
            }

            bDXT10Header = true;
        }

        // setup the pointers in the process request
        *header = hdr;
        ptrdiff_t offset = sizeof(uint32_t) + sizeof(DDS_HEADER)
            + (bDXT10Header ? sizeof(DDS_HEADER_DXT10) : 0);
        *bitData = data + offset;
        *bitSize = dataSize - offset;

        return true;
    }


    //--------------------------------------------------------------------------------------
#define ISBITMASK( r,g,b,a ) ( ddpf.RBitMask == r && ddpf.GBitMask == g && ddpf.BBitMask == b && ddpf.ABitMask == a )

    static GHDXGIFormat GetDXGIFormat(const DDS_PIXELFORMAT& ddpf)
    {
        if (ddpf.flags & DDS_RGB)
        {
            // Note that sRGB formats are written using the "DX10" extended header

            switch (ddpf.RGBBitCount)
            {
            case 32:
                if (ISBITMASK(0x000000ff, 0x0000ff00, 0x00ff0000, 0xff000000))
                {
                    return DF_R8G8B8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0xff000000))
                {
                    return DF_B8G8R8A8_UNORM;
                }

                if (ISBITMASK(0x00ff0000, 0x0000ff00, 0x000000ff, 0x00000000))
                {
                    return DF_B8G8R8X8_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000000ff,0x0000ff00,0x00ff0000,0x00000000) aka D3DFMT_X8B8G8R8

                // Note that many common DDS reader/writers (including D3DX) swap the
                // the RED/BLUE masks for 10:10:10:2 formats. We assumme
                // below that the 'backwards' header mask is being used since it is most
                // likely written by D3DX. The more robust solution is to use the 'DX10'
                // header extension and specify the DXGI_FORMAT_R10G10B10A2_UNORM format directly

                // For 'correct' writers, this should be 0x000003ff,0x000ffc00,0x3ff00000 for RGB data
                if (ISBITMASK(0x3ff00000, 0x000ffc00, 0x000003ff, 0xc0000000))
                {
                    return DF_R10G10B10A2_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x000003ff,0x000ffc00,0x3ff00000,0xc0000000) aka D3DFMT_A2R10G10B10

                if (ISBITMASK(0x0000ffff, 0xffff0000, 0x00000000, 0x00000000))
                {
                    return DF_R16G16_UNORM;
                }

                if (ISBITMASK(0xffffffff, 0x00000000, 0x00000000, 0x00000000))
                {
                    // Only 32-bit color channel format in D3D9 was R32F
                    return DF_R32_FLOAT; // D3DX writes this out as a FourCC of 114
                }
                break;

            case 24:
                // No 24bpp DXGI formats aka D3DFMT_R8G8B8
                break;

            case 16:
                if (ISBITMASK(0x7c00, 0x03e0, 0x001f, 0x8000))
                {
                    return DF_B5G5R5A1_UNORM;
                }
                if (ISBITMASK(0xf800, 0x07e0, 0x001f, 0x0000))
                {
                    return DF_B5G6R5_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x7c00,0x03e0,0x001f,0x0000) aka D3DFMT_X1R5G5B5

#ifdef DXGI_1_2_FORMATS
                if (ISBITMASK(0x0f00, 0x00f0, 0x000f, 0xf000))
                {
                    return DXGI_FORMAT_B4G4R4A4_UNORM;
                }

                // No DXGI format maps to ISBITMASK(0x0f00,0x00f0,0x000f,0x0000) aka D3DFMT_X4R4G4B4
#endif

            // No 3:3:2, 3:3:2:8, or paletted DXGI formats aka D3DFMT_A8R3G3B2, D3DFMT_R3G3B2, D3DFMT_P8, D3DFMT_A8P8, etc.
                break;
            }
        }
        else if (ddpf.flags & DDS_LUMINANCE)
        {
            if (8 == ddpf.RGBBitCount)
            {
                if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x00000000))
                {
                    return DF_R8_UNORM; // D3DX10/11 writes this out as DX10 extension
                }

                // No DXGI format maps to ISBITMASK(0x0f,0x00,0x00,0xf0) aka D3DFMT_A4L4
            }

            if (16 == ddpf.RGBBitCount)
            {
                if (ISBITMASK(0x0000ffff, 0x00000000, 0x00000000, 0x00000000))
                {
                    return DF_R16_UNORM; // D3DX10/11 writes this out as DX10 extension
                }
                if (ISBITMASK(0x000000ff, 0x00000000, 0x00000000, 0x0000ff00))
                {
                    return DF_R8G8_UNORM; // D3DX10/11 writes this out as DX10 extension
                }
            }
        }
        else if (ddpf.flags & DDS_ALPHA)
        {
            if (8 == ddpf.RGBBitCount)
            {
                return DF_A8_UNORM;
            }
        }
        else if (ddpf.flags & DDS_FOURCC)
        {
            if (MAKEFOURCC('D', 'X', 'T', '1') == ddpf.fourCC)
            {
                return DF_BC1_UNORM;
            }
            if (MAKEFOURCC('D', 'X', 'T', '3') == ddpf.fourCC)
            {
                return DF_BC2_UNORM;
            }
            if (MAKEFOURCC('D', 'X', 'T', '5') == ddpf.fourCC)
            {
                return DF_BC3_UNORM;
            }

            // While pre-mulitplied alpha isn't directly supported by the DXGI formats,
            // they are basically the same as these BC formats so they can be mapped
            if (MAKEFOURCC('D', 'X', 'T', '2') == ddpf.fourCC)
            {
                return DF_BC2_UNORM;
            }
            if (MAKEFOURCC('D', 'X', 'T', '4') == ddpf.fourCC)
            {
                return DF_BC3_UNORM;
            }

            if (MAKEFOURCC('A', 'T', 'I', '1') == ddpf.fourCC)
            {
                return DF_BC4_UNORM;
            }
            if (MAKEFOURCC('B', 'C', '4', 'U') == ddpf.fourCC)
            {
                return DF_BC4_UNORM;
            }
            if (MAKEFOURCC('B', 'C', '4', 'S') == ddpf.fourCC)
            {
                return DF_BC4_SNORM;
            }

            if (MAKEFOURCC('A', 'T', 'I', '2') == ddpf.fourCC)
            {
                return DF_BC5_UNORM;
            }
            if (MAKEFOURCC('B', 'C', '5', 'U') == ddpf.fourCC)
            {
                return DF_BC5_UNORM;
            }
            if (MAKEFOURCC('B', 'C', '5', 'S') == ddpf.fourCC)
            {
                return DF_BC5_SNORM;
            }

            // BC6H and BC7 are written using the "DX10" extended header

            if (MAKEFOURCC('R', 'G', 'B', 'G') == ddpf.fourCC)
            {
                return DF_R8G8_B8G8_UNORM;
            }
            if (MAKEFOURCC('G', 'R', 'G', 'B') == ddpf.fourCC)
            {
                return DF_G8R8_G8B8_UNORM;
            }

            // Check for D3DFORMAT enums being set here
            switch (ddpf.fourCC)
            {
            case 36: // D3DFMT_A16B16G16R16
                return DF_R16G16B16A16_UNORM;

            case 110: // D3DFMT_Q16W16V16U16
                return DF_R16G16B16A16_SNORM;

            case 111: // D3DFMT_R16F
                return DF_R16_FLOAT;

            case 112: // D3DFMT_G16R16F
                return DF_R16G16_FLOAT;

            case 113: // D3DFMT_A16B16G16R16F
                return DF_R16G16B16A16_FLOAT;

            case 114: // D3DFMT_R32F
                return DF_R32_FLOAT;

            case 115: // D3DFMT_G32R32F
                return DF_R32G32_FLOAT;

            case 116: // D3DFMT_A32B32G32R32F
                return DF_R32G32B32A32_FLOAT;
            }
        }

        return DF_UNKNOWN;
    }

    //--------------------------------------------------------------------------------------
    bool FillInitData(DDSDesc& desc,
        _In_ size_t maxsize,
        _In_ size_t bitSize,
        _In_bytecount_(bitSize) const uint8_t* bitData,
        _Out_cap_(mipCount* arraySize) SubresourceData* initData)
    {
        if (!bitData || !initData)
            return false;

        desc.skipMip = 0;
        desc.twidth = 0;
        desc.theight = 0;
        desc.tdepth = 0;

        size_t NumBytes = 0;
        size_t RowBytes = 0;
        size_t NumRows = 0;
        const uint8_t* pSrcBits = bitData;
        const uint8_t* pEndBits = bitData + bitSize;

        size_t index = 0;
        for (size_t j = 0; j < desc.arraySize; j++)
        {
            size_t w = desc.width;
            size_t h = desc.height;
            size_t d = desc.depth;
            for (size_t i = 0; i < desc.mipCount; i++)
            {
                GHDXGIUtil::getSurfaceInfo(w,
                    h,
                    (GHDXGIFormat)desc.format,
                    &NumBytes,
                    &RowBytes,
                    &NumRows
                );

                if ((desc.mipCount <= 1) || !maxsize || (w <= maxsize && h <= maxsize && d <= maxsize))
                {
                    if (!desc.twidth)
                    {
                        desc.twidth = w;
                        desc.theight = h;
                        desc.tdepth = d;
                    }

                    initData[index].pSysMem = (const void*)pSrcBits;
                    initData[index].SysMemPitch = static_cast<uint32_t>(RowBytes);
                    initData[index].SysMemSlicePitch = static_cast<uint32_t>(NumBytes);
                    ++index;
                }
                else
                    ++desc.skipMip;

                if (pSrcBits + (NumBytes * d) > pEndBits)
                {
                    return false;
                }

                pSrcBits += NumBytes * d;

                w = w >> 1;
                h = h >> 1;
                d = d >> 1;
                if (w == 0)
                {
                    w = 1;
                }
                if (h == 0)
                {
                    h = 1;
                }
                if (d == 0)
                {
                    d = 1;
                }
            }
        }

        return (index > 0) ? true : false;
    }

    bool parseHeaderMemory(
        _In_bytecount_(ddsDataSize) const uint8_t* ddsData,
        _In_ size_t ddsDataSize,
        const DDS_HEADER*& outHeader,
        ptrdiff_t& outDataOffset
    )
    {
        // Validate DDS file in memory
        if (ddsDataSize < (sizeof(uint32_t) + sizeof(GHDDSUtil::DDS_HEADER)))
        {
            return false;
        }

        uint32_t dwMagicNumber = *(const uint32_t*)(ddsData);
        if (dwMagicNumber != DDS_MAGIC)
        {
            return false;
        }

        outHeader = reinterpret_cast<const GHDDSUtil::DDS_HEADER*>(ddsData + sizeof(uint32_t));

        // Verify header to validate DDS file
        if (outHeader->size != sizeof(GHDDSUtil::DDS_HEADER) ||
            outHeader->ddspf.size != sizeof(GHDDSUtil::DDS_PIXELFORMAT))
        {
            return false;
        }

        // Check for DX10 extension
        bool bDXT10Header = false;
        if ((outHeader->ddspf.flags & DDS_FOURCC) &&
            (MAKEFOURCC('D', 'X', '1', '0') == outHeader->ddspf.fourCC))
        {
            // Must be long enough for both headers and magic value
            if (ddsDataSize < (sizeof(GHDDSUtil::DDS_HEADER) + sizeof(uint32_t) + sizeof(GHDDSUtil::DDS_HEADER_DXT10)))
            {
                return false;
            }

            bDXT10Header = true;
        }

        outDataOffset = sizeof(uint32_t)
            + sizeof(GHDDSUtil::DDS_HEADER)
            + (bDXT10Header ? sizeof(GHDDSUtil::DDS_HEADER_DXT10) : 0);

        return true;
    }

    bool validateAndParseHeader(const DDS_HEADER& header, DDSDesc& desc)
    {
        desc.width = header.width;
        desc.height = header.height;
        desc.depth = header.depth;

        desc.resDim = DDS_RESOURCE_DIMENSION_UNKNOWN;
        desc.arraySize = 1;
        desc.format = DF_UNKNOWN;
        desc.isCubeMap = false;

        size_t mipCount = header.mipMapCount;
        if (0 == mipCount)
        {
            mipCount = 1;
        }
        desc.mipCount = mipCount;

        if ((header.ddspf.flags & DDS_FOURCC) &&
            (MAKEFOURCC('D', 'X', '1', '0') == header.ddspf.fourCC))
        {
            const DDS_HEADER_DXT10* d3d10ext = reinterpret_cast<const DDS_HEADER_DXT10*>((const char*)&header + sizeof(DDS_HEADER));

            desc.arraySize = d3d10ext->arraySize;
            if (desc.arraySize == 0)
            {
                return false;
            }

            if (GHDXGIUtil::bitsPerPixel((GHDXGIFormat)d3d10ext->dxgiFormat) == 0)
            {
                return false;
            }

            desc.format = d3d10ext->dxgiFormat;

            switch (d3d10ext->resourceDimension)
            {
            case DDS_RESOURCE_DIMENSION_TEXTURE1D:
                // D3DX writes 1D textures with a fixed Height of 1
                if ((header.flags & DDS_HEIGHT) && desc.height != 1)
                {
                    return false;
                }
                desc.height = desc.depth = 1;
                break;

            case DDS_RESOURCE_DIMENSION_TEXTURE2D:
                if (d3d10ext->miscFlag & DDS_RESOURCE_MISC_TEXTURECUBE)
                {
                    desc.arraySize *= 6;
                    desc.isCubeMap = true;
                }
                desc.depth = 1;
                break;

            case DDS_RESOURCE_DIMENSION_TEXTURE3D:
                if (!(header.flags & DDS_HEADER_FLAGS_VOLUME))
                {
                    return false;
                }

                if (desc.arraySize > 1)
                {
                    return false;
                }
                break;

            default:
                return false;
            }

            desc.resDim = d3d10ext->resourceDimension;
        }
        else
        {
            desc.format = GHDDSUtil::GetDXGIFormat(header.ddspf);

            if (desc.format == DF_UNKNOWN)
            {
                return false;
            }

            if (header.flags & DDS_HEADER_FLAGS_VOLUME)
            {
                desc.resDim = DDS_RESOURCE_DIMENSION_TEXTURE3D;
            }
            else
            {
                if (header.caps2 & DDS_CUBEMAP)
                {
                    // We require all six faces to be defined
                    if ((header.caps2 & DDS_CUBEMAP_ALLFACES) != DDS_CUBEMAP_ALLFACES)
                    {
                        return false;
                    }

                    desc.arraySize = 6;
                    desc.isCubeMap = true;
                }

                desc.depth = 1;
                desc.resDim = DDS_RESOURCE_DIMENSION_TEXTURE2D;

                // Note there's no way for a legacy Direct3D 9 DDS to express a '1D' texture
            }

            assert(GHDXGIUtil::bitsPerPixel((GHDXGIFormat)desc.format) != 0);
        }

        // Bound sizes (for security purposes we don't trust DDS file metadata larger than the D3D 11.x hardware requirements)
        if (desc.mipCount > DDS_REQ_MIP_LEVELS)
        {
            return false;
        }

        switch (desc.resDim)
        {
        case DDS_RESOURCE_DIMENSION_TEXTURE1D:
            if ((desc.arraySize > DDS_REQ_TEXTURE1D_ARRAY_AXIS_DIMENSION) ||
                (desc.width > DDS_REQ_TEXTURE1D_U_DIMENSION))
            {
                return false;
            }
            break;

        case DDS_RESOURCE_DIMENSION_TEXTURE2D:
            if (desc.isCubeMap)
            {
                // This is the right bound because we set arraySize to (NumCubes*6) above
                if ((desc.arraySize > DDS_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                    (desc.width > DDS_REQ_TEXTURECUBE_DIMENSION) ||
                    (desc.height > DDS_REQ_TEXTURECUBE_DIMENSION))
                {
                    return false;
                }
            }
            else if ((desc.arraySize > DDS_REQ_TEXTURE2D_ARRAY_AXIS_DIMENSION) ||
                (desc.width > DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION) ||
                (desc.height > DDS_REQ_TEXTURE2D_U_OR_V_DIMENSION))
            {
                return false;
            }
            break;

        case DDS_RESOURCE_DIMENSION_TEXTURE3D:
            if ((desc.arraySize > 1) ||
                (desc.width > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (desc.height > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION) ||
                (desc.depth > DDS_REQ_TEXTURE3D_U_V_OR_W_DIMENSION))
            {
                return false;
            }
            break;
        }

        return true;
    }

};