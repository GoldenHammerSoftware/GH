#pragma once

#include "GHDX12Include.h"
#include "GHUtils/GHResource.h"
#include "GHString/GHString.h"
#include "Render/GHShaderParamList.h"

class GHShaderDX12
{
public:
	GHShaderDX12(void* data, size_t dataSize, GHResourcePtr<GHShaderParamList>* params, const char* name);
	~GHShaderDX12(void);

	const D3D12_SHADER_BYTECODE& getBytecode(void) const { return mBytecode; }
	const GHShaderParamList* getParams(void) const;

private:
	D3D12_SHADER_BYTECODE mBytecode;
	GHResourcePtr<GHShaderParamList>* mParams;
	GHString mName;
};

typedef GHResourcePtr<GHShaderDX12> GHShaderResource;