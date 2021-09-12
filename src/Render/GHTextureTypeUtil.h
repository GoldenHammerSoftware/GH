#pragma once

#include "GHTextureFormat.h"
#include "GHTextureChannelType.h"

// Utility functions for dealing with gles flags for textures
// It is entirely expected that these functions or arguments will need to evolve in the future.
// Example: srgb input, handling 565 textures.
class GHTextureTypeUtil
{
public:
	static bool isCompressedType(const GHTextureFormat::Enum type);
	// GLenum
	static uint32_t getGLChannelType(const GHTextureChannelType::Enum type);
	static GHTextureChannelType::Enum getGHChannelType(uint32_t glType);
	// GLint
	static int32_t getGLFormat(const GHTextureFormat::Enum format);
	static GHTextureFormat::Enum getGHFormat(int32_t glFormat);
    
    // width in pixel of a compression block for a texture format.
    // 1 if uncompressed.
    static size_t getBlockWidthPixels(const GHTextureFormat::Enum type);
    static size_t getPixelSizeBytes(const GHTextureFormat::Enum type);
};

