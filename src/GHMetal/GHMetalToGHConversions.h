#pragma once

#include "GHVertexStreamFormat.h"
#include "GHMetalShaderStructs.h"
#include "GHMDesc.h"
#include "GHMaterialCallbackType.h"
#include "GHTextureFormat.h"

// Dumping ground for converting from gh to metal enums.
namespace GHMetal
{

static MTLVertexFormat mtlVertexFormatForComp(const GHVertexStreamFormat::ComponentEntry& comp)
{
    if (comp.mType == GHVertexComponentType::CT_BYTE)
    {
        if (comp.mCount == 1)
        {
            return MTLVertexFormatChar;
        }
        if (comp.mCount == 2)
        {
            return MTLVertexFormatChar2;
        }
        if (comp.mCount == 3)
        {
            return MTLVertexFormatChar3;
        }
        if (comp.mCount == 4)
        {
            return MTLVertexFormatChar4;
        }
    }
    if (comp.mType == GHVertexComponentType::CT_UBYTE)
    {
        if (comp.mCount == 1)
        {
            return MTLVertexFormatUChar;
        }
        if (comp.mCount == 2)
        {
            return MTLVertexFormatUChar2;
        }
        if (comp.mCount == 3)
        {
            return MTLVertexFormatUChar3;
        }
        if (comp.mCount == 4)
        {
            return MTLVertexFormatUChar4;
        }
    }
    if (comp.mType == GHVertexComponentType::CT_SHORT)
    {
        if (comp.mCount == 1)
        {
            return MTLVertexFormatShort;
        }
        if (comp.mCount == 2)
        {
            return MTLVertexFormatShort2;
        }
        if (comp.mCount == 3)
        {
            return MTLVertexFormatShort3;
        }
        if (comp.mCount == 4)
        {
            return MTLVertexFormatShort4;
        }
    }
    if (comp.mType == GHVertexComponentType::CT_FLOAT)
    {
        if (comp.mCount == 1)
        {
            return MTLVertexFormatFloat;
        }
        if (comp.mCount == 2)
        {
            return MTLVertexFormatFloat2;
        }
        if (comp.mCount == 3)
        {
            return MTLVertexFormatFloat3;
        }
        if (comp.mCount == 4)
        {
            return MTLVertexFormatFloat4;
        }
    }
    return MTLVertexFormatInvalid;
}

static int getMetalComponentIndex(GHVertexComponentShaderID::Enum ghVal)
{
    // Todo: make this mapping into a header that can be shared with the .metal files.
    switch(ghVal)
    {
        case GHVertexComponentShaderID::SI_POS:
            return GHMTL_AI_POS;
        case GHVertexComponentShaderID::SI_NORMAL:
            return GHMTL_AI_NORMAL;
        case GHVertexComponentShaderID::SI_TANGENT:
            return GHMTL_AI_TANGENT;
        case GHVertexComponentShaderID::SI_DIFFUSE:
            return GHMTL_AI_DIFFUSE;
        case GHVertexComponentShaderID::SI_SPECULAR:
            return GHMTL_AI_SPECULAR;
        case GHVertexComponentShaderID::SI_BONEIDX:
            return GHMTL_AI_BONEIDX;
        case GHVertexComponentShaderID::SI_UV1:
            return GHMTL_AI_UV0;
        case GHVertexComponentShaderID::SI_UV2:
            return GHMTL_AI_UV1;
        default:
            return GHMTL_AI_MAX;
    }
}

static GHMaterialCallbackType::Enum getGHBufferIndex(GHMTL_BufferIndex mtlIdx)
{
    switch(mtlIdx)
    {
        case GHMTL_BI_VPERFRAME:
        case GHMTL_BI_PPERFRAME:
            return GHMaterialCallbackType::CT_PERFRAME;
        case GHMTL_BI_VPERTRANS:
        case GHMTL_BI_PPERTRANS:
            return GHMaterialCallbackType::CT_PERTRANS;
        case GHMTL_BI_VPERGEO:
        case GHMTL_BI_PPERGEO:
            return GHMaterialCallbackType::CT_PERGEO;
        case GHMTL_BI_VPERENT:
        case GHMTL_BI_PPERENT:
            return GHMaterialCallbackType::CT_PERENT;
        default:
            return GHMaterialCallbackType::CT_MAX;
    };
}

static GHMTL_BufferIndex getMetalBufferIndex(GHMaterialCallbackType::Enum ghIdx)
{
    switch(ghIdx)
    {
        case GHMaterialCallbackType::CT_PERFRAME:
            return GHMTL_BI_VPERFRAME;
        case GHMaterialCallbackType::CT_PERTRANS:
            return GHMTL_BI_VPERTRANS;
        case GHMaterialCallbackType::CT_PERGEO:
            return GHMTL_BI_VPERGEO;
        case GHMaterialCallbackType::CT_PERENT:
            return GHMTL_BI_VPERENT;
        default:
            return (GHMTL_BufferIndex)0;
    };
}

static MTLBlendFactor metalBlendForGHBlend(const GHMDesc::BlendMode& ghMode)
{
    if (ghMode == GHMDesc::BM_ALPHA)
    {
        return MTLBlendFactorSourceAlpha;
    }
    else if (ghMode == GHMDesc::BM_INVALPHA)
    {
        return MTLBlendFactorOneMinusSourceAlpha;
    }
    return MTLBlendFactorOne;
}

static MTLWinding metalWindingForGHCull(const GHMDesc::CullMode& ghMode)
{
    if (ghMode == GHMDesc::CM_CCW)
    {
        return MTLWindingCounterClockwise;
    }
    return MTLWindingClockwise;
}

static MTLCullMode metalCullModeForGHCull(const GHMDesc::CullMode& ghMode)
{
    if (ghMode == GHMDesc::CM_NOCULL)
    {
        return MTLCullModeNone;
    }
    return MTLCullModeFront;
}

static MTLPixelFormat metalPixelFormatForGH(GHTextureFormat::Enum ghFormat)
{
    switch (ghFormat)
    {
        case GHTextureFormat::TF_RGBA8:
            return MTLPixelFormatRGBA8Unorm;
            break;
        case GHTextureFormat::TF_PVRTC_2BPP_RGB:
            return MTLPixelFormatPVRTC_RGB_2BPP;
            break;
        case GHTextureFormat::TF_PVRTC_2BPP_RGBA:
            return MTLPixelFormatPVRTC_RGBA_2BPP;
            break;
        case GHTextureFormat::TF_PVRTC_4BPP_RGB:
            return MTLPixelFormatPVRTC_RGB_4BPP;
            break;
        case GHTextureFormat::TF_PVRTC_4BPP_RGBA:
            return MTLPixelFormatPVRTC_RGBA_4BPP;
            break;
        case GHTextureFormat::TF_COMPRESSED_RGB8_ETC2:
            return MTLPixelFormatETC2_RGB8;
            break;
        case GHTextureFormat::TF_COMPRESSED_RGBA8_ETC2_EAC:
            return MTLPixelFormatEAC_RGBA8;
            break;
        case GHTextureFormat::TF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
            return MTLPixelFormatETC2_RGB8A1;
            break;
        // todo: astc formats
        // todo: srgb.
        default:
            assert(0 && "Unsupported texture format, check if metal supports it.");
    };
    return MTLPixelFormatRGBA8Unorm;
}

static GHTextureFormat::Enum GHPixelFormatForMetal(MTLPixelFormat metalFormat)
{
    switch (metalFormat)
    {
        case MTLPixelFormatRGBA8Unorm:
        case MTLPixelFormatRGBA8Unorm_sRGB:
            return GHTextureFormat::TF_RGBA8;
            break;
        case MTLPixelFormatBGRA8Unorm:
        case MTLPixelFormatBGRA8Unorm_sRGB:
            return GHTextureFormat::TF_BGRA8;
            break;
        case MTLPixelFormatPVRTC_RGB_2BPP:
            return GHTextureFormat::TF_PVRTC_2BPP_RGB;
            break;
        case MTLPixelFormatPVRTC_RGBA_2BPP:
            return GHTextureFormat::TF_PVRTC_2BPP_RGBA;
            break;
        case MTLPixelFormatPVRTC_RGB_4BPP:
            return GHTextureFormat::TF_PVRTC_4BPP_RGB;
            break;
        case MTLPixelFormatPVRTC_RGBA_4BPP:
            return GHTextureFormat::TF_PVRTC_4BPP_RGBA;
            break;
        case MTLPixelFormatETC2_RGB8:
            return GHTextureFormat::TF_COMPRESSED_RGB8_ETC2;
            break;
        case MTLPixelFormatEAC_RGBA8:
            return GHTextureFormat::TF_COMPRESSED_RGBA8_ETC2_EAC;
            break;
        case MTLPixelFormatETC2_RGB8A1:
            return GHTextureFormat::TF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
            break;
        // todo: astc formats
        // todo: srgb.
        default:
            assert(0 && "Unsupported texture format, check if metal supports it.");
    };
    return GHTextureFormat::TF_RGBA8;
}

};
