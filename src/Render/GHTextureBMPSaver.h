#pragma once

#include "GHTextureFormat.h"

class GHFileOpener;
class GHTexture;


class GHTextureBMPSaver
{
public:
	GHTextureBMPSaver(const GHFileOpener& fileOpener);

	bool saveTexture(GHTexture& texture, const char* filename, GHTextureFormat::Enum srcFormat, GHTextureFormat::Enum dstFormat) const;

private:
	const GHFileOpener& mFileOpener;

};
