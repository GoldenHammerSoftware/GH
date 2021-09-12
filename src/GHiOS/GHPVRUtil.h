// Copyright 2010 Golden Hammer Software
#pragma once
// no ogl include!
#include <stdint.h>
#include <vector>

class GHiOSFilenameCreator;
class GHFileOpener;

class GHPVRUtil
{
public:
	enum
	{
		kPVRTextureFlagTypePVRTC_2 = 24,
		kPVRTextureFlagTypePVRTC_4
	};
    
	enum InternalFormatType
	{
		IF_RGBA_4BPP,
		IF_RGB_4BPP,
		IF_RGBA_2BPP,
	};
	
	typedef struct _PVRTexHeader
	{
		uint32_t headerLength;
		uint32_t height;
		uint32_t width;
		uint32_t numMipmaps;
		uint32_t flags;
		uint32_t dataLength;
		uint32_t bpp;
		uint32_t bitmaskRed;
		uint32_t bitmaskGreen;
		uint32_t bitmaskBlue;
		uint32_t bitmaskAlpha;
		uint32_t pvrTag;
		uint32_t numSurfs;
	} PVRTexHeader;
	
	typedef struct 
	{
        std::vector<void*> _imageData;
        std::vector<size_t> _imageDataSize;
		unsigned int _name;
		uint32_t _width, _height;
		int _internalFormat;
		BOOL _hasAlpha;
		BOOL _hasMips;
	} PVRTextureDesc;
	
public:
	static bool unpackData(void* data, size_t dataSize, PVRTextureDesc& tex, 
						   int rgba_4b_enum, int rgb_4b_enum, 
						   int rgba_2b_enum, int rgb_2b_enum);
    static bool loadPVRData(const char* filename, GHiOSFilenameCreator& fileNameCreator,
                            const GHFileOpener& fOpener, void*& data, size_t& dataSize);
};
