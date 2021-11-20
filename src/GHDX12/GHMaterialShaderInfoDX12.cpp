#include "GHMaterialShaderInfoDX12.h"
#include "GHShaderDX12.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHTexture.h"

GHMaterialShaderInfoDX12::GHMaterialShaderInfoDX12(GHRenderDeviceDX12& device, GHShaderResource* shader)
	: mShader(shader)
{
	assert(mShader);
	mShader->acquire();
	createConstantBuffers(device);
}

GHMaterialShaderInfoDX12::~GHMaterialShaderInfoDX12(void)
{
	mShader->release();
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		delete mCBuffers[i];
		for (unsigned int j = 0; j < mTextures[i].size(); ++j) {
			delete mTextures[i][j];
		}
	}
}

void GHMaterialShaderInfoDX12::createConstantBuffers(GHRenderDeviceDX12& device)
{
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		unsigned int bufSize = mShader->get()->getParamList()->getBufferSize((GHMaterialCallbackType::Enum)i);
		// create a cbuffer even if size 0 in order to make sure the root descriptor matches.
		mCBuffers[i] = new GHDX12CBuffer(device, bufSize);
	}
	// extract the texture info
	const std::vector<GHShaderParamList::Param>& vsparams = mShader->get()->getParamList()->getParams();
	for (unsigned int i = 0; i < vsparams.size(); ++i)
	{
		if (vsparams[i].mHandleType != GHMaterialParamHandle::HT_TEXTURE) continue;
		TextureSlot* tex = new TextureSlot(vsparams[i].mRegister);
		mTextures[(int)vsparams[i].mCBType].push_back(tex);
	}
}

GHMaterialShaderInfoDX12::TextureSlot::TextureSlot(unsigned int registerId)
	: mRegister(registerId)
{
}

GHMaterialShaderInfoDX12::TextureSlot::~TextureSlot(void)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, 0);
}

void GHMaterialShaderInfoDX12::TextureSlot::setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, tex);
	mWrapMode = wrapMode;
	// dx11 would create a sampler here.
}

