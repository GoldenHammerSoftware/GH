#include "GHMaterialShaderInfoDX12.h"
#include "GHShaderDX12.h"

GHMaterialShaderInfoDX12::GHMaterialShaderInfoDX12(GHShaderResource* shader)
	: mShader(shader)
{
	assert(mShader);
	mShader->acquire();
	createConstantBuffers();
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

void GHMaterialShaderInfoDX12::createConstantBuffers(void)
{
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		unsigned int vsBufSize = mShader->get()->getParamList()->getBufferSize((GHMaterialCallbackType::Enum)i);
		if (vsBufSize) 
		{
			mCBuffers[i] = new ConstantBufferInfo(vsBufSize);
			// todo: dx buffers.
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

GHMaterialShaderInfoDX12::ConstantBufferInfo::ConstantBufferInfo(size_t size)
	: mDataSize(size)
{
	mData = new byte[mDataSize];
	// todo: dx buffers
}

GHMaterialShaderInfoDX12::ConstantBufferInfo::~ConstantBufferInfo(void)
{
	delete[] mData;
}
