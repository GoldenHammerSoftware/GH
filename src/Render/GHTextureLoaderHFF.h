// Copyright Golden Hammer Software
#pragma once

#include "GHUtils/GHResourceLoader.h"
#include <stdio.h>

class GHFileOpener;

class GHTextureLoaderHFF : public GHResourceLoader
{
public:
	struct HFFHeader
	{
		HFFHeader(void)
		{
			::sprintf(mFileTypeMarker, "L3DT");
			::sprintf(mTextMarker, "HFF_v1.0");
			mBinaryMarker = 300;
			mDataOffset = 64; // not sizeof due to packing.
			mFloatingPointFlag = 1;
			mTileSize = 0;
			mWrapFlag = 70;
			mDataSize = 4; // float
			mVerticalScale = 1.0f;
			mVerticalOffset = 0.0f;
			::sprintf(mReserved, "F_v1.0");
		}

		char mFileTypeMarker[4];
		unsigned short mBinaryMarker;
		char mTextMarker[8];
		unsigned short mDataOffset;
		unsigned int mMapWidth;
		unsigned int mMapHeight;
		unsigned char mDataSize;
		unsigned char mFloatingPointFlag;
		float mVerticalScale;
		float mVerticalOffset;
		float mHorizontalScale;
		unsigned short mTileSize;
		unsigned char mWrapFlag;
		char mReserved[23];
	};

public:
    GHTextureLoaderHFF(const GHFileOpener& fileOpener);
    
    virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData=0);
    
private:
	void readHeader(const char* buffer, HFFHeader& header);

private:
    const GHFileOpener& mFileOpener;
};
