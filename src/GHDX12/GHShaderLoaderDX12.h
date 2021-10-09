#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHFileOpener;
class GHShaderParamListLoader;

class GHShaderLoaderDX12 : public GHResourceLoader
{
public:
	GHShaderLoaderDX12(GHFileOpener& fileOpener, const GHShaderParamListLoader& paramListLoader);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0) override;

private:
	GHFileOpener& mFileOpener;
	const GHShaderParamListLoader& mParamListLoader;
};