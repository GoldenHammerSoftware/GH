#include "GHShaderDX12.h"

GHShaderDX12::GHShaderDX12(void* data, size_t dataSize)
{
	mBytecode.pShaderBytecode = data;
	mBytecode.BytecodeLength = dataSize;
}

GHShaderDX12::~GHShaderDX12(void)
{
	delete mBytecode.pShaderBytecode;
}
