#include "GHShaderLoaderDX12.h"
#include "GHPlatform/GHFileOpener.h"

GHShaderLoaderDX12::GHShaderLoaderDX12(GHFileOpener& fileOpener)
	: mFileOpener(fileOpener)
{
}

GHResource* GHShaderLoaderDX12::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	return 0;
}
