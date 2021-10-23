#pragma once

#include "GHShaderDX12.h"
#include "GHDX12CBuffer.h"

class GHRenderDeviceDX12;

class GHMaterialShaderInfoDX12
{
public:
    GHMaterialShaderInfoDX12(GHRenderDeviceDX12& device, GHShaderResource* shader);
    ~GHMaterialShaderInfoDX12(void);

private:
    void createConstantBuffers(GHRenderDeviceDX12& device);

public:
	GHShaderResource* mShader{ 0 };

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