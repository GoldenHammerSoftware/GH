// Copyright Golden Hammer Software
#include "GHHeightFieldHFFSaveModifier.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHHeightField.h"

GHHeightFieldHFFSaveModifier::GHHeightFieldHFFSaveModifier(const GHHeightField& hf,
	const GHFileOpener& fileOpener,
	const char* fileName)
: mHF(hf)
, mFileOpener(fileOpener)
, mFileName(fileName, GHString::CHT_COPY)
{
}

void GHHeightFieldHFFSaveModifier::activate(void)
{
	GHFile* file = mFileOpener.openFile(mFileName.getChars(), GHFile::FT_BINARY, GHFile::FM_WRITEONLY);
	if (!file) {
		GHDebugMessage::outputString("Failed to open HFF for writing");
		return;
	}

	GHTextureLoaderHFF::HFFHeader header;
	mHF.getDimensions(header.mMapWidth, header.mMapHeight);
	header.mHorizontalScale = mHF.getDistBetweenNodes();
	writeHeader(header, *file);

	const float* heights = mHF.getHeights();
	file->writeBuffer(heights, sizeof(float)*header.mMapWidth*header.mMapWidth);

	file->closeFile();
	delete file;
}

void GHHeightFieldHFFSaveModifier::deactivate(void)
{
}

void GHHeightFieldHFFSaveModifier::writeHeader(GHTextureLoaderHFF::HFFHeader& header, 
	GHFile& file)
{
	file.writeBuffer(header.mFileTypeMarker, 4);
	file.writeBuffer(&header.mBinaryMarker, sizeof(unsigned short));
	file.writeBuffer(header.mTextMarker, 8);
	file.writeBuffer(&header.mDataOffset, sizeof(unsigned short));
	file.writeBuffer(&header.mMapWidth, sizeof(unsigned int));
	file.writeBuffer(&header.mMapHeight, sizeof(unsigned int));
	file.writeBuffer(&header.mDataSize, 1);
	file.writeBuffer(&header.mFloatingPointFlag, 1);
	file.writeBuffer(&header.mVerticalScale, sizeof(float));
	file.writeBuffer(&header.mVerticalOffset, sizeof(float));
	file.writeBuffer(&header.mHorizontalScale, sizeof(float));
	file.writeBuffer(&header.mTileSize, sizeof(unsigned short));
	file.writeBuffer(&header.mWrapFlag, 1);
	file.writeBuffer(header.mReserved, 23);
}
