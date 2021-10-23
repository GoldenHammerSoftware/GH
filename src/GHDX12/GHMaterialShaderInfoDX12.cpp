#include "GHMaterialShaderInfoDX12.h"
#include "GHShaderDX12.h"

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
		unsigned int vsBufSize = mShader->get()->getParamList()->getBufferSize((GHMaterialCallbackType::Enum)i);
		if (vsBufSize) 
		{
			mCBuffers[i] = new GHDX12CBuffer(device, vsBufSize);
		}
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
}

void GHMaterialShaderInfoDX12::TextureSlot::setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode)
{
}

