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
		delete[] mCBuffers[i].mData;
	}
}

void GHMaterialShaderInfoDX12::createConstantBuffers(void)
{
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		unsigned int vsBufSize = mShader->get()->getParamList()->getBufferSize((GHMaterialCallbackType::Enum)i);
		if (vsBufSize) 
		{
			mCBuffers[i].mDataSize = vsBufSize;
			mCBuffers[i].mData = new byte[vsBufSize];
			// todo: dx buffers.
		}
	}
	// extract the texture info
	const std::vector<GHShaderParamList::Param>& vsparams = mShader->get()->getParamList()->getParams();
	for (unsigned int i = 0; i < vsparams.size(); ++i)
	{
		if (vsparams[i].mHandleType != GHMaterialParamHandle::HT_TEXTURE) continue;
		// todo:  handle textures.
	}
}
