#include "GHShaderDX12.h"

GHShaderDX12::GHShaderDX12(void* data, size_t dataSize, GHResourcePtr<GHShaderParamList>* params, const char* name)
	: mParams(params)
	, mName(name, GHString::CHT_COPY)
{
	mBytecode.pShaderBytecode = data;
	mBytecode.BytecodeLength = dataSize;
	if (mParams) mParams->acquire();
}

GHShaderDX12::~GHShaderDX12(void)
{
	delete mBytecode.pShaderBytecode;
	if (mParams) mParams->release();
}

const GHShaderParamList* GHShaderDX12::getParamList(void) const
{
	return mParams->get();
}