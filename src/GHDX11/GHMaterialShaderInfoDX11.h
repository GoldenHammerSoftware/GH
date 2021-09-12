// Copyright Golden Hammer Software
#pragma once

#include "GHDX11Include.h"
#include "GHMDesc.h"
#include "GHMaterialCallbackType.h"
#include "GHShaderDX11.h"
#include <vector>

class GHRenderDeviceDX11;
class GHTexture;

// struct to hold the necessary info to render a material with a shader
class GHMaterialShaderInfoDX11
{
public:
	GHMaterialShaderInfoDX11(GHRenderDeviceDX11& device, GHShaderResource* shader);
	~GHMaterialShaderInfoDX11(void);

	void reinit(void);

private:
	void createCBuffers(GHRenderDeviceDX11& device);

public:
	GHRenderDeviceDX11& mDevice;
	GHShaderResource* mShader;

	class TextureSlot
	{
	public:
		TextureSlot(GHRenderDeviceDX11& device, unsigned int registerId);
		~TextureSlot(void);

		void setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode);
		GHTexture* getTexture(void) { return mTexture; }
		Microsoft::WRL::ComPtr<ID3D11SamplerState> getSampler(void) { return mSampler; }
		unsigned int getRegister(void) const { return mRegister; }

		void reinit(void);

	private:
		void createSampler(void);

	private:
		GHRenderDeviceDX11& mDevice;
		unsigned int mRegister;
		GHTexture* mTexture;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSampler;
		GHMDesc::WrapMode mWrapMode;
	};
	typedef std::vector<TextureSlot*> TextureList;
	TextureList mTextures[GHMaterialCallbackType::CT_MAX];

	// the constant buffers we will send to the shaders
	struct CBuffer
	{
		CBuffer(unsigned int bufferSize, GHRenderDeviceDX11& device);
		~CBuffer(void);

		void updateD3DBuffer(GHRenderDeviceDX11& device);
		void createD3DBuffer(GHRenderDeviceDX11& device);
		void reinit(GHRenderDeviceDX11& device);

		byte* mBuffer;
		unsigned int mBufferSize;
		Microsoft::WRL::ComPtr<ID3D11Buffer> mD3DBuffer;
	};
	CBuffer* mCBuffers[GHMaterialCallbackType::CT_MAX];
};
