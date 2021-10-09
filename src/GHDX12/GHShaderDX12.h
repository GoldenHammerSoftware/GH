#pragma once

#include "GHDX12Include.h"
#include "GHUtils/GHResource.h"

class GHShaderDX12
{
public:
	GHShaderDX12(void* data, size_t dataSize);
	~GHShaderDX12(void);

	const D3D12_SHADER_BYTECODE& getBytecode(void) const { return mBytecode; }

private:
	D3D12_SHADER_BYTECODE mBytecode;
};

typedef GHResourcePtr<GHShaderDX12> GHShaderResource;