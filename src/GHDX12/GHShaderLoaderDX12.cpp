#include "GHShaderLoaderDX12.h"
#include "GHPlatform/GHFileOpener.h"
#include "GHUtils/GHResource.h"
#include "Render/GHShaderParamList.h"
#include "Render/GHShaderParamListLoader.h"
#include "GHShaderDX12.h"
#include "GHPlatform/GHDebugMessage.h"

GHShaderLoaderDX12::GHShaderLoaderDX12(GHFileOpener& fileOpener, const GHShaderParamListLoader& paramListLoader)
	: mFileOpener(fileOpener)
	, mParamListLoader(paramListLoader)
{
}

GHResource* GHShaderLoaderDX12::loadFile(const char* filename, GHPropertyContainer* extraData)
{
	GHResourcePtr<GHShaderParamList>* params = mParamListLoader.loadParams(filename);
	params->acquire();

	// dx11 loaded high/med/low variants of the shaders here.

	GHFile* file = mFileOpener.openFile(filename, GHFile::FT_BINARY, GHFile::FM_READONLY);
	if (!file) 
	{
		GHDebugMessage::outputString("Failed to load shader %s", filename);
		return 0;
	}
	char* fileBuf = 0;
	size_t fileLen = 0;
	file->readIntoBuffer();
	file->getFileBuffer(fileBuf, fileLen);
	file->releaseFileBuffer();

	GHShaderDX12* ret = new GHShaderDX12(fileBuf, fileLen, params, filename);
	delete file;

	// we've passed the params to ret to claim so we can release it.
	params->release();

	return new GHShaderResource(ret);
}
