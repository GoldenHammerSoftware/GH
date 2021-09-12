// Copyright Golden Hammer Software
#pragma once

class GHFileOpener;
class GHTextureDataFactory;
struct GHTextureData;

// Generates a GHTextureData* from a filename.
class GHTextureDataLoader
{
public:
	GHTextureDataLoader(const GHFileOpener& fileOpener, const GHTextureDataFactory& dataFactory);

	GHTextureData* load(const char* filename) const;

private:
	const GHFileOpener& mFileOpener;
	const GHTextureDataFactory& mDataFactory;
};
