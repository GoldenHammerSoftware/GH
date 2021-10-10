#pragma once

#include "GHShaderDX12.h"

class GHMaterialShaderInfoDX12
{
public:
    GHMaterialShaderInfoDX12(GHShaderResource* shader);
    ~GHMaterialShaderInfoDX12(void);

private:
    void createConstantBuffers(void);

private:
	GHShaderResource* mShader{ 0 };

    // The material's view of one constant buffer.
    struct ConstantBufferInfo
    {
        void* mData{ nullptr };
        size_t mDataSize{ 0 };
        // todo: add the dx12 view.
    };
    // One constant buffer per GHMaterialCallbackType.
    ConstantBufferInfo mCBuffers[GHMaterialCallbackType::CT_MAX];

    // todo: textures.
};