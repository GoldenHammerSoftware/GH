#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHFileOpener;

class GHShaderLoaderDX12 : public GHResourceLoader
{
public:
	GHShaderLoaderDX12(GHFileOpener& fileOpener);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0) override;

private:
	GHFileOpener& mFileOpener;
};