#pragma once

#include "GHShaderDX12.h"

class GHMaterialShaderInfoDX12
{
public:
    GHMaterialShaderInfoDX12(GHShaderResource* shader);
    ~GHMaterialShaderInfoDX12(void);

private:
    void createConstantBuffers(void);

public:
	GHShaderResource* mShader{ 0 };

    // The material's view of one constant buffer.
    struct ConstantBufferInfo
    {
	public:
		ConstantBufferInfo(size_t size);
		~ConstantBufferInfo(void);

        byte* mData{ nullptr };
        size_t mDataSize{ 0 };
        // todo: add the dx12 view.
    };
    // One constant buffer per GHMaterialCallbackType.
    ConstantBufferInfo* mCBuffers[GHMaterialCallbackType::CT_MAX];

	class TextureSlot
	{
	public:
		TextureSlot(unsigned int registerId);
		~TextureSlot(void);

		void setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode);
		GHTexture* getTexture(void) { return mTexture; }
		unsigned int getRegister(void) const { return mRegister; }

		// todo: sampler.
		// todo: reinit.

	private:
		unsigned int mRegister;
		GHTexture* mTexture;
		GHMDesc::WrapMode mWrapMode;
	};
	typedef std::vector<TextureSlot*> TextureList;
	TextureList mTextures[GHMaterialCallbackType::CT_MAX];
};