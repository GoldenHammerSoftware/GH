#pragma once

#include "GHShaderDX12.h"
#include "GHDX12CBuffer.h"
#include "Render/GHShaderType.h"

class GHRenderDeviceDX12;

class GHMaterialShaderInfoDX12
{
public:
    GHMaterialShaderInfoDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>* descriptorHeaps, GHShaderResource* shader, GHShaderType::Enum shaderType);
    ~GHMaterialShaderInfoDX12(void);

private:
    void createConstantBuffers(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>* descriptorHeaps);

public:
	GHShaderResource* mShader{ 0 };
	GHShaderType::Enum mShaderType;

    // One constant buffer per GHMaterialCallbackType.
    GHDX12CBuffer* mCBuffers[GHMaterialCallbackType::CT_MAX];

	class TextureSlot
	{
	public:
		TextureSlot(unsigned int registerId);
		~TextureSlot(void);

		void setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode);
		GHTexture* getTexture(void) { return mTexture; }
		unsigned int getRegister(void) const { return mRegister; }

	private:
		unsigned int mRegister{ 0 };
		GHTexture* mTexture{ nullptr };
		GHMDesc::WrapMode mWrapMode{ GHMDesc::WrapMode::WM_WRAP };
	};
	typedef std::vector<TextureSlot*> TextureList;
	TextureList mTextures[GHMaterialCallbackType::CT_MAX];
};