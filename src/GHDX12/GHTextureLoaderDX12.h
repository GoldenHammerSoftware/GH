#pragma once

#include "GHUtils/GHResourceLoader.h"

class GHTextureLoaderDX12 : public GHResourceLoader
{
public:
	GHTextureLoaderDX12(void);
	~GHTextureLoaderDX12(void);

	virtual GHResource* loadFile(const char* filename, GHPropertyContainer* extraData = 0) override;
	virtual GHResource* loadMemory(void* mem, size_t memSize, GHPropertyContainer* extraData = 0) override;
};
