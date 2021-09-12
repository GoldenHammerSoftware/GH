#include "GHTextureTypeUtil.h"
#include "GHGLTypes.h"

bool GHTextureTypeUtil::isCompressedType(const GHTextureFormat::Enum type)
{
	return (int)type > (int)GHTextureFormat::TF_MAX_UNCOMPRESSED;
}

uint32_t GHTextureTypeUtil::getGLChannelType(const GHTextureChannelType::Enum type)
{
	switch (type)
	{
	case GHTextureChannelType::TC_UBYTE:
		return GL_UNSIGNED_BYTE;
	case GHTextureChannelType::TC_BYTE:
		return GL_BYTE;
	case GHTextureChannelType::TC_USHORT:
		return GL_UNSIGNED_SHORT; // todo: 4444, 565, 5551?
	case GHTextureChannelType::TC_SHORT:
		return GL_SHORT;
	case GHTextureChannelType::TC_UINT:
		return GL_UNSIGNED_INT;
	case GHTextureChannelType::TC_INT:
		return GL_INT;
	case GHTextureChannelType::TC_FLOAT:
		return GL_FLOAT;
	case GHTextureChannelType::TC_DOUBLE:
		return GL_DOUBLE;
	// todo: I don't think these have enums?
	case GHTextureChannelType::TC_UBYTE_NORM:
	case GHTextureChannelType::TC_BYTE_NORM:
	case GHTextureChannelType::TC_USHORT_NORM:
	case GHTextureChannelType::TC_SHORT_NORM:
	case GHTextureChannelType::TC_UINT_NORM:
	case GHTextureChannelType::TC_INT_NORM:
	default:
		return GL_UNSIGNED_BYTE;
	}
}

GHTextureChannelType::Enum GHTextureTypeUtil::getGHChannelType(uint32_t glType)
{
	switch (glType)
	{
	case GL_BYTE:
		return GHTextureChannelType::TC_BYTE;
	case GL_UNSIGNED_BYTE:
		return GHTextureChannelType::TC_UBYTE;
	case GL_SHORT:
		return GHTextureChannelType::TC_SHORT;
	case GL_UNSIGNED_SHORT:
		return GHTextureChannelType::TC_USHORT;
	case GL_INT:
		return GHTextureChannelType::TC_INT;
	case GL_UNSIGNED_INT:
		return GHTextureChannelType::TC_UINT;
	case GL_FLOAT:
		return GHTextureChannelType::TC_FLOAT;
	case GL_DOUBLE:
		return GHTextureChannelType::TC_DOUBLE;
	case GL_2_BYTES:
		return GHTextureChannelType::TC_SHORT; // ? half?
	case GL_4_BYTES:
		return GHTextureChannelType::TC_DOUBLE; // ?
	case GL_3_BYTES:
	default:
		return GHTextureChannelType::TC_UNKNOWN;
	}
}

int32_t GHTextureTypeUtil::getGLFormat(const GHTextureFormat::Enum format)
{
	switch (format)
	{
	case GHTextureFormat::TF_RGB8:
		return GL_RGB;
	case GHTextureFormat::TF_RGBA8:
		return GL_RGBA;

	case GHTextureFormat::TF_DEPTH:
		return GL_DEPTH_COMPONENT;

	case GHTextureFormat::TF_PVRTC_2BPP_RGB:
		return COMPRESSED_RGB_PVRTC_2BPPV1_IMG;
	case GHTextureFormat::TF_PVRTC_2BPP_RGBA:
		return COMPRESSED_RGBA_PVRTC_2BPPV1_IMG;
	case GHTextureFormat::TF_PVRTC_4BPP_RGB:
		return COMPRESSED_RGB_PVRTC_4BPPV1_IMG;
	case GHTextureFormat::TF_PVRTC_4BPP_RGBA:
		return COMPRESSED_RGBA_PVRTC_4BPPV1_IMG;
	case GHTextureFormat::TF_PVRTC2_2BPP:
		return COMPRESSED_RGBA_PVRTC_2BPPV2_IMG;
	case GHTextureFormat::TF_PVRTC2_4BPP:
		return COMPRESSED_RGBA_PVRTC_4BPPV2_IMG;

	// todo: srgb version ex COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
	case GHTextureFormat::TF_ASTC_4x4:
	{
		return COMPRESSED_RGBA_ASTC_4x4_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR;
	}
	case GHTextureFormat::TF_ASTC_5x4:
	{
		return COMPRESSED_RGBA_ASTC_5x4_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR;
	}
	case GHTextureFormat::TF_ASTC_5x5:
	{
		return COMPRESSED_RGBA_ASTC_5x5_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR;
	}
	case GHTextureFormat::TF_ASTC_6x5:
	{
		return COMPRESSED_RGBA_ASTC_6x5_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR;
	}
	case GHTextureFormat::TF_ASTC_6x6:
	{
		return COMPRESSED_RGBA_ASTC_6x6_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR;
	}
	case GHTextureFormat::TF_ASTC_8x5:
	{
		return COMPRESSED_RGBA_ASTC_8x5_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR;
	}
	case GHTextureFormat::TF_ASTC_8x6:
	{
		return COMPRESSED_RGBA_ASTC_8x6_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR;
	}
	case GHTextureFormat::TF_ASTC_8x8:
	{
		return COMPRESSED_RGBA_ASTC_8x8_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR;
	}
	case GHTextureFormat::TF_ASTC_10x5:
	{
		return COMPRESSED_RGBA_ASTC_10x5_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR;
	}
	case GHTextureFormat::TF_ASTC_10x6:
	{
		return COMPRESSED_RGBA_ASTC_10x6_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR;
	}
	case GHTextureFormat::TF_ASTC_10x8:
	{
		return COMPRESSED_RGBA_ASTC_10x8_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR;
	}
	case GHTextureFormat::TF_ASTC_10x10:
	{
		return COMPRESSED_RGBA_ASTC_10x10_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR;
	}
	case GHTextureFormat::TF_ASTC_12x10:
	{
		return COMPRESSED_RGBA_ASTC_12x10_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR;
	}
	case GHTextureFormat::TF_ASTC_12x12:
	{
		return COMPRESSED_RGBA_ASTC_12x12_KHR;
		//return COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR;
	}
	// todo: 3d astc ex TF_ASTC_6x6x5

	case GHTextureFormat::TF_ETC1_RGB8_OES:
		return ETC1_RGB8_OES;
	case GHTextureFormat::TF_COMPRESSED_RGB8_ETC2:
		return COMPRESSED_RGB8_ETC2;
	case GHTextureFormat::TF_COMPRESSED_SRGB8_ETC2:
		return COMPRESSED_SRGB8_ETC2;
	case GHTextureFormat::TF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		return COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
	case GHTextureFormat::TF_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		return COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
	case GHTextureFormat::TF_COMPRESSED_RGBA8_ETC2_EAC:
		return COMPRESSED_RGBA8_ETC2_EAC;
	case GHTextureFormat::TF_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		return COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
	case GHTextureFormat::TF_COMPRESSED_R11_EAC:
		return COMPRESSED_R11_EAC;
	case GHTextureFormat::TF_COMPRESSED_SIGNED_R11_EAC:
		return COMPRESSED_SIGNED_R11_EAC;
	case GHTextureFormat::TF_COMPRESSED_RG11_EAC:
		return COMPRESSED_RG11_EAC;
	case GHTextureFormat::TF_COMPRESSED_SIGNED_RG11_EAC:
			return COMPRESSED_SIGNED_RG11_EAC;

	default:
		// todo: all the others.  assert? probably.
		return GL_RGBA;
	}
}

GHTextureFormat::Enum GHTextureTypeUtil::getGHFormat(int32_t glFormat)
{
	switch (glFormat)
	{
	case GL_RGB:
		return GHTextureFormat::TF_RGB8;
	case GL_RGBA:
		return GHTextureFormat::TF_RGBA8;

	case COMPRESSED_RGB_PVRTC_2BPPV1_IMG:
		return GHTextureFormat::TF_PVRTC_2BPP_RGB;
	case COMPRESSED_RGBA_PVRTC_2BPPV1_IMG:
		return GHTextureFormat::TF_PVRTC_2BPP_RGBA;
	case COMPRESSED_RGB_PVRTC_4BPPV1_IMG:
		return GHTextureFormat::TF_PVRTC_4BPP_RGB;
	case COMPRESSED_RGBA_PVRTC_4BPPV1_IMG:
		return GHTextureFormat::TF_PVRTC_4BPP_RGBA;
	case COMPRESSED_RGBA_PVRTC_2BPPV2_IMG:
		return GHTextureFormat::TF_PVRTC2_2BPP;
	case COMPRESSED_RGBA_PVRTC_4BPPV2_IMG:
		return GHTextureFormat::TF_PVRTC2_4BPP;

	case COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR:
	case COMPRESSED_RGBA_ASTC_4x4_KHR:
	{
		return GHTextureFormat::TF_ASTC_4x4;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_5x4_KHR:
	case COMPRESSED_RGBA_ASTC_5x4_KHR:
	{
		return GHTextureFormat::TF_ASTC_5x4;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_5x5_KHR:
	case COMPRESSED_RGBA_ASTC_5x5_KHR:
	{
		return GHTextureFormat::TF_ASTC_5x5;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_6x5_KHR:
	case COMPRESSED_RGBA_ASTC_6x5_KHR:
	{
		return GHTextureFormat::TF_ASTC_6x5;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_6x6_KHR:
	case COMPRESSED_RGBA_ASTC_6x6_KHR:
	{
		return GHTextureFormat::TF_ASTC_6x6;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_8x5_KHR:
	case COMPRESSED_RGBA_ASTC_8x5_KHR:
	{
		return GHTextureFormat::TF_ASTC_8x5;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_8x6_KHR:
	case COMPRESSED_RGBA_ASTC_8x6_KHR:
	{
		return GHTextureFormat::TF_ASTC_8x6;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_8x8_KHR:
	case COMPRESSED_RGBA_ASTC_8x8_KHR:
	{
		return GHTextureFormat::TF_ASTC_8x8;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_10x5_KHR:
	case COMPRESSED_RGBA_ASTC_10x5_KHR:
	{
		return GHTextureFormat::TF_ASTC_10x5;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_10x6_KHR:
	case COMPRESSED_RGBA_ASTC_10x6_KHR:
	{
		return GHTextureFormat::TF_ASTC_10x6;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_10x8_KHR:
	case COMPRESSED_RGBA_ASTC_10x8_KHR:
	{
		return GHTextureFormat::TF_ASTC_10x8;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_10x10_KHR:
	case COMPRESSED_RGBA_ASTC_10x10_KHR:
	{
		return GHTextureFormat::TF_ASTC_10x10;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_12x10_KHR:
	case COMPRESSED_RGBA_ASTC_12x10_KHR:
	{
		return GHTextureFormat::TF_ASTC_12x10;
	}
	case COMPRESSED_SRGB8_ALPHA8_ASTC_12x12_KHR:
	case COMPRESSED_RGBA_ASTC_12x12_KHR:
	{
		return GHTextureFormat::TF_ASTC_12x12;
	}
	// todo: 3d astc ex TF_ASTC_6x6x5

	case ETC1_RGB8_OES:
		return GHTextureFormat::TF_ETC1_RGB8_OES;
	case COMPRESSED_RGB8_ETC2:
		return GHTextureFormat::TF_COMPRESSED_RGB8_ETC2;
	case COMPRESSED_SRGB8_ETC2:
		return GHTextureFormat::TF_COMPRESSED_SRGB8_ETC2;
	case COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		return GHTextureFormat::TF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2;
	case COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
		return GHTextureFormat::TF_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2;
	case COMPRESSED_RGBA8_ETC2_EAC:
		return GHTextureFormat::TF_COMPRESSED_RGBA8_ETC2_EAC;
	case COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
		return GHTextureFormat::TF_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC;
	case COMPRESSED_R11_EAC:
		return GHTextureFormat::TF_COMPRESSED_R11_EAC;
	case COMPRESSED_SIGNED_R11_EAC:
		return GHTextureFormat::TF_COMPRESSED_SIGNED_R11_EAC;
	case COMPRESSED_RG11_EAC:
		return GHTextureFormat::TF_COMPRESSED_RG11_EAC;
	case COMPRESSED_SIGNED_RG11_EAC:
		return GHTextureFormat::TF_COMPRESSED_SIGNED_RG11_EAC;

	default:
		return GHTextureFormat::TF_UNKNOWN;
	}
}

size_t GHTextureTypeUtil::getBlockWidthPixels(const GHTextureFormat::Enum type)
{
    if (!isCompressedType(type))
    {
        return 1;
    }
    switch (type)
    {
    case GHTextureFormat::TF_PVRTC_2BPP_RGB:
        return 8;
    case GHTextureFormat::TF_PVRTC_2BPP_RGBA:
        return 8;
    case GHTextureFormat::TF_PVRTC_4BPP_RGB:
        return 4;
    case GHTextureFormat::TF_PVRTC_4BPP_RGBA:
        return 4;
    case GHTextureFormat::TF_PVRTC2_2BPP:
        return 8;
    case GHTextureFormat::TF_PVRTC2_4BPP:
        return 4;

    // todo: srgb version ex COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR
    case GHTextureFormat::TF_ASTC_4x4:
    {
        return 4;
    }
    case GHTextureFormat::TF_ASTC_5x4:
    {
        return 5;
    }
    case GHTextureFormat::TF_ASTC_5x5:
    {
        return 5;
    }
    case GHTextureFormat::TF_ASTC_6x5:
    {
        return 6;
    }
    case GHTextureFormat::TF_ASTC_6x6:
    {
        return 6;
    }
    case GHTextureFormat::TF_ASTC_8x5:
    {
        return 8;
    }
    case GHTextureFormat::TF_ASTC_8x6:
    {
        return 8;
    }
    case GHTextureFormat::TF_ASTC_8x8:
    {
        return 8;
    }
    case GHTextureFormat::TF_ASTC_10x5:
    {
        return 10;
    }
    case GHTextureFormat::TF_ASTC_10x6:
    {
        return 10;
    }
    case GHTextureFormat::TF_ASTC_10x8:
    {
        return 10;
    }
    case GHTextureFormat::TF_ASTC_10x10:
    {
        return 10;
    }
    case GHTextureFormat::TF_ASTC_12x10:
    {
        return 12;
    }
    case GHTextureFormat::TF_ASTC_12x12:
    {
        return 12;
    }
    // todo: 3d astc ex TF_ASTC_6x6x5

    case GHTextureFormat::TF_ETC1_RGB8_OES:
    case GHTextureFormat::TF_COMPRESSED_RGB8_ETC2:
    case GHTextureFormat::TF_COMPRESSED_SRGB8_ETC2:
    case GHTextureFormat::TF_COMPRESSED_RGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case GHTextureFormat::TF_COMPRESSED_SRGB8_PUNCHTHROUGH_ALPHA1_ETC2:
    case GHTextureFormat::TF_COMPRESSED_RGBA8_ETC2_EAC:
    case GHTextureFormat::TF_COMPRESSED_SRGB8_ALPHA8_ETC2_EAC:
        return 4;
            
    case GHTextureFormat::TF_COMPRESSED_R11_EAC:
        return 1;
    case GHTextureFormat::TF_COMPRESSED_SIGNED_R11_EAC:
        return 1;
    case GHTextureFormat::TF_COMPRESSED_RG11_EAC:
        return 1;
    case GHTextureFormat::TF_COMPRESSED_SIGNED_RG11_EAC:
        return 1;

    default:
        // todo: all the others.  assert? probably.
        return GL_RGBA;
    }
}

size_t GHTextureTypeUtil::getPixelSizeBytes(const GHTextureFormat::Enum type)
{
    switch (type)
    {
    case GHTextureFormat::TF_RGB8:
    case GHTextureFormat::TF_BGR8:
        return 3;
    case GHTextureFormat::TF_RGBA8:
    case GHTextureFormat::TF_BGRA8:
        return 4;
    default:
        // todo: all the others.  assert? probably.
        return 0;
    }
}
