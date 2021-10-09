// Copyright Golden Hammer Software
#include "GHMaterialShaderInfoDX11.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHShaderParamList.h"
#include "GHRenderDeviceDX11.h"
#include "GHTexture.h"

GHMaterialShaderInfoDX11::GHMaterialShaderInfoDX11(GHRenderDeviceDX11& device, GHShaderResource* shader)
: mShader(0)
, mDevice(device)
{
	GHResource::changePointer((GHRefCounted*&)mShader, shader);
	createCBuffers(device);
}

GHMaterialShaderInfoDX11::~GHMaterialShaderInfoDX11(void)
{
	GHResource::changePointer((GHRefCounted*&)mShader, 0);

	for (unsigned int i = (int)GHMaterialCallbackType::CT_PERFRAME; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		delete mCBuffers[i];
		for (unsigned int j = 0; j < mTextures[i].size(); ++j) {
			delete mTextures[i][j];
		}
	}
}

void GHMaterialShaderInfoDX11::reinit(void)
{
	for (int i = 0; i < GHMaterialCallbackType::CT_MAX; ++i)
	{
		CBuffer* currBuffer = mCBuffers[i];
		if (currBuffer)
		{
			currBuffer->reinit(mDevice);
		}

		TextureList& texList = mTextures[i];
		for (size_t texIdx = 0; texIdx < texList.size(); ++texIdx)
		{
			texList[texIdx]->reinit();
		}
	}
}

void GHMaterialShaderInfoDX11::createCBuffers(GHRenderDeviceDX11& device)
{
	for (int i = 0; i < (int)GHMaterialCallbackType::CT_MAX; ++i)
	{
		unsigned int vsBufSize = mShader->get()->getParamList().getBufferSize((GHMaterialCallbackType::Enum)i);
		if (vsBufSize) {
			mCBuffers[i] = new CBuffer(vsBufSize, device);
		}
		else {
			mCBuffers[i] = 0;
		}
	}
	// extract the texture info
	const std::vector<GHShaderParamList::Param>& vsparams = mShader->get()->getParamList().getParams();
	for (unsigned int i = 0; i < vsparams.size(); ++i)
	{
		if (vsparams[i].mHandleType != GHMaterialParamHandle::HT_TEXTURE) continue;
		TextureSlot* tex = new TextureSlot(device, vsparams[i].mRegister);
		mTextures[(int)vsparams[i].mCBType].push_back(tex);
	}
}

GHMaterialShaderInfoDX11::CBuffer::CBuffer(unsigned int bufferSize, GHRenderDeviceDX11& device)
: mBufferSize(bufferSize)
{
	// cbuffers must be a size multiple of 16 bytes.
	if (mBufferSize % 16) {
		mBufferSize += 16-(mBufferSize%16);
	}
	mBuffer = new byte[mBufferSize];
	::memset(mBuffer, 0, mBufferSize);

	createD3DBuffer(device);
}

GHMaterialShaderInfoDX11::CBuffer::~CBuffer(void)
{
	delete [] mBuffer;
}

void GHMaterialShaderInfoDX11::CBuffer::createD3DBuffer(GHRenderDeviceDX11& device)
{
    D3D11_BUFFER_DESC constantBufferDesc = {0};
    constantBufferDesc.ByteWidth = mBufferSize;
    constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
    constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    constantBufferDesc.MiscFlags = 0;
    constantBufferDesc.StructureByteStride = 0;
    HRESULT cbRes = device.getD3DDevice()->CreateBuffer(
		&constantBufferDesc,
		nullptr,
		mD3DBuffer.GetAddressOf()
		);
	if (FAILED(cbRes) || !mD3DBuffer.Get()) {
		GHDebugMessage::outputString("Failed to make cbuffer");
	}
}

void GHMaterialShaderInfoDX11::CBuffer::updateD3DBuffer(GHRenderDeviceDX11& device)
{
	if (!mD3DBuffer.Get()) return;

	void* voidBuffer = 0;
	D3D11_MAPPED_SUBRESOURCE ms;
	device.getD3DContext()->Map(mD3DBuffer.Get(), NULL, D3D11_MAP_WRITE_DISCARD, NULL, &ms); 
	voidBuffer = ms.pData;

	::memcpy(voidBuffer, mBuffer, mBufferSize);

	device.getD3DContext()->Unmap(mD3DBuffer.Get(), NULL);
}

void GHMaterialShaderInfoDX11::CBuffer::reinit(GHRenderDeviceDX11& device)
{
	mD3DBuffer = nullptr;
	createD3DBuffer(device);
	updateD3DBuffer(device);
}

GHMaterialShaderInfoDX11::TextureSlot::TextureSlot(GHRenderDeviceDX11& device, unsigned int registerId)
: mDevice(device)
, mRegister(registerId)
, mTexture(0)
, mSampler(nullptr)
{
}

GHMaterialShaderInfoDX11::TextureSlot::~TextureSlot(void)
{
	GHResource::changePointer((GHRefCounted*&)mTexture, 0);
}

void GHMaterialShaderInfoDX11::TextureSlot::createSampler(void)
{
	mSampler = nullptr;

	D3D11_SAMPLER_DESC samplerDesc;
	ZeroMemory(&samplerDesc, sizeof(samplerDesc));

	//samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	// "feature level 9.1 specifies a maximum anisotropy of 2x. Because the Anisotropic Texture Filtering variant attempts to use 16x anisotropy exclusively, playback fails when frame analysis is run on a feature-level 9.1 device. Contemporary devices that are affected by this limitation include the ARM-based Surface RT and Surface 2 Windows tablets. Older GPUs that might still be found in some computers can also be affected, but they're widely considered to be obsolete and are increasingly uncommon."
	samplerDesc.MaxAnisotropy = D3D11_REQ_MAXANISOTROPY;

	// Specify how texture coordinates outside of the range 0..1 are resolved.
	if (mWrapMode == GHMDesc::WM_WRAP)
	{
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	}
	else
	{
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	}

	// Use no special MIP clamping or bias.
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	// Don't use a comparison function.
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;

	// Border address mode is not used, so this parameter is ignored.
	samplerDesc.BorderColor[0] = 0.0f;
	samplerDesc.BorderColor[1] = 0.0f;
	samplerDesc.BorderColor[2] = 0.0f;
	samplerDesc.BorderColor[3] = 0.0f;

	HRESULT samplerRes = mDevice.getD3DDevice()->CreateSamplerState(
        &samplerDesc,
        mSampler.GetAddressOf()
        );
    if (FAILED(samplerRes)) {
		GHDebugMessage::outputString("Failed to create sampler state");
	}
}

void GHMaterialShaderInfoDX11::TextureSlot::setTexture(GHTexture* tex, GHMDesc::WrapMode wrapMode)
{
	GHMDesc::WrapMode oldWrapMode = mWrapMode;

	GHResource::changePointer((GHRefCounted*&)mTexture, tex);
	mWrapMode = wrapMode;
	if (!mTexture) return;

	if (mWrapMode == oldWrapMode && mSampler) {
		// no need to create a new sampler.
		return;
	}

	createSampler();
}

void GHMaterialShaderInfoDX11::TextureSlot::reinit(void)
{
	mSampler = nullptr;
	setTexture(mTexture, mWrapMode);
}
