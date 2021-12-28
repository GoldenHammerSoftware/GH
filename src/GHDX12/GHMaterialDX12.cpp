#include "GHMaterialDX12.h"
#include "Render/GHMDesc.h"
#include "GHMaterialShaderInfoDX12.h"
#include "GHMaterialParamHandleDX12.h"
#include "Render/GHMaterialCallback.h"
#include "Render/GHBillboardTransformAdjuster.h"
#include "GHMath/GHTransform.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHRenderDeviceDX12.h"
#include "Render/GHVertexBuffer.h"
#include "GHVBBlitterIndexDX12.h"
#include "GHTextureDX12.h"
#include "GHDX12MaterialHeapPool.h"
#include "GHDX12PSOPool.h"
#include "GHMath/GHHash.h"

GHMaterialDX12::GHMaterialDX12(GHRenderDeviceDX12& device, GHDX12MaterialHeapPool& heapPool, GHDX12PSOPool& psoPool, GHMDesc* desc, GHShaderResource* vs, GHShaderResource* ps)
	: mDevice(device)
	, mDesc(desc)
	, mHeapPool(heapPool)
	, mPSOPool(psoPool)
{
	mShaders[GHShaderType::ST_VERTEX] = new GHMaterialShaderInfoDX12(device, mHeapPool, vs);
	mShaders[GHShaderType::ST_PIXEL] = new GHMaterialShaderInfoDX12(device, mHeapPool, ps);

	GHMDesc::ParamHandles* descParamHandles = desc->initMaterial(*this);
	desc->applyDefaultArgs(*this, *descParamHandles);
	desc->applyTextures(*this, *descParamHandles);
	delete descParamHandles;

	mIsAlpha = mDesc->mAlphaBlend;

	updateMaterialPsoHash();

	// todo: GHRenderProperties::DEVICEREINIT
}

GHMaterialDX12::~GHMaterialDX12(void)
{
	delete mDesc;
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType) {
		delete mShaders[shaderType];
	}
}

void GHMaterialDX12::beginMaterial(const GHViewInfo& viewInfo)
{
	// might not end up being necessary.
	mDescriptorsDirty = true;

	applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
	applyDXArgs(GHMaterialCallbackType::CT_PERFRAME);
}

void GHMaterialDX12::beginVB(const GHVertexBuffer& vb)
{
	Microsoft::WRL::ComPtr<ID3D12PipelineState> pso = createPSO(vb);
	mDevice.getRenderCommandList()->SetPipelineState(pso.Get());
}

void GHMaterialDX12::updateDescriptorHeap(void)
{
	if (!mDescriptorsDirty) return;

	mDescriptorHeap = mHeapPool.getDescriptorHeap();
	mSamplerHeap = mHeapPool.getSamplerHeap();
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		for (int cbType = 0; cbType < (int)GHMaterialCallbackType::CT_MAX; ++cbType)
		{
			if (!mShaders[shaderType]->mCBuffers[cbType]) continue;
			if (!mShaders[shaderType]->mCBuffers[cbType]->getSize()) continue;
			size_t indexInHeap = ((int)GHMaterialCallbackType::CT_MAX * shaderType) + cbType;
			mShaders[shaderType]->mCBuffers[cbType]->createSRV(mDescriptorHeap, indexInHeap);
		}
	}

	for (int cbType = 0; cbType < (int)GHMaterialCallbackType::CT_MAX; ++cbType)
	{
		for (unsigned int i = 0; i < mShaders[GHShaderType::ST_PIXEL]->mTextures[cbType].size(); ++i)
		{
			GHMaterialShaderInfoDX12::TextureSlot* currTex = mShaders[GHShaderType::ST_PIXEL]->mTextures[cbType][i];
			if (!currTex->getTexture()) continue;

			GHTextureDX12* currTexDX = (GHTextureDX12*)currTex->getTexture();
			currTexDX->bind(mDescriptorHeap, 8, currTex->getRegister());
			currTexDX->createSampler(mSamplerHeap, currTex->getRegister(), currTex->getWrapMode());
		}
	}
	mDescriptorsDirty = false;
}

void GHMaterialDX12::preBlit(void)
{
	updateDescriptorHeap();

	// The heap will ultimately end up needing to be unique for each time one of our values change.  
	// This will make everything draw with only the final draw call's settings.

	ID3D12DescriptorHeap* heaps[] = { mDescriptorHeap.Get(), mSamplerHeap.Get() };
	mDevice.getRenderCommandList()->SetDescriptorHeaps(2, heaps);

	const UINT cbvSrvDescriptorSize = mDevice.getDXDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	D3D12_GPU_DESCRIPTOR_HANDLE heapOffsetHandle = mDescriptorHeap->GetGPUDescriptorHandleForHeapStart();

	mDevice.getRenderCommandList()->SetGraphicsRootDescriptorTable(0, heapOffsetHandle);
	heapOffsetHandle.ptr += (4 * cbvSrvDescriptorSize);
	mDevice.getRenderCommandList()->SetGraphicsRootDescriptorTable(1, heapOffsetHandle);
	heapOffsetHandle.ptr += (4 * cbvSrvDescriptorSize);
	mDevice.getRenderCommandList()->SetGraphicsRootDescriptorTable(2, heapOffsetHandle);

	mDevice.getRenderCommandList()->SetGraphicsRootDescriptorTable(3, mSamplerHeap->GetGPUDescriptorHandleForHeapStart());
}

void GHMaterialDX12::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(geoData, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERGEO);
}

void GHMaterialDX12::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
	GHMaterialCallback::CBVal_PerObj cbArg(entData, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERENT);
}

void GHMaterialDX12::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
	GHBillboardTransformAdjuster transAdjuster;
	GHTransform scratchTransform;
	const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);

	GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
	applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
	applyDXArgs(GHMaterialCallbackType::CT_PERTRANS);
}

void GHMaterialDX12::endMaterial(void)
{
	// reset anything we don't want to carry over to the next draw.
}

GHMaterialParamHandle* GHMaterialDX12::getParamHandle(const char* paramName)
{
	GHMaterialParamHandleDX12* ret = 0;
	GHString paramString(mDesc->checkParamAlias(paramName), GHString::CHT_REFERENCE);
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		const GHShaderParamList& paramList = *mShaders[shaderType]->mShader->get()->getParamList();
		const GHShaderParamList::Param* param = paramList.getParam(paramString);
		if (param)
		{
			if (!ret) ret = new GHMaterialParamHandleDX12;
			ret->addShaderHandle(mShaders[shaderType], param);
		}
	}
	return ret;
}

void GHMaterialDX12::applyDXArgs(GHMaterialCallbackType::Enum type)
{
	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		if (!mShaders[shaderType]->mCBuffers[type]) continue;
		if (!mShaders[shaderType]->mCBuffers[type]->getSize()) continue;
		mShaders[shaderType]->mCBuffers[type]->updateFrameData();
		mDescriptorsDirty = true;
	}
}

Microsoft::WRL::ComPtr<ID3D12PipelineState> GHMaterialDX12::createPSO(const GHVertexBuffer& vb)
{
	return mPSOPool.getPSO(mMaterialPsoHash, *mDesc, *mShaders[GHShaderType::ST_VERTEX]->mShader->get(), *mShaders[GHShaderType::ST_PIXEL]->mShader->get(), vb, SWAP_BUFFER_FORMAT, DXGI_FORMAT_D32_FLOAT);
}

void GHMaterialDX12::updateMaterialPsoHash(void)
{
	mMaterialPsoHash = 0;

	for (unsigned int shaderType = 0; shaderType < GHShaderType::ST_MAX; ++shaderType)
	{
		if (!mShaders[shaderType]->mShader->get()) continue;
		GHHash::hash_combine(mMaterialPsoHash, mShaders[shaderType]->mShader->get());
	}

	GHHash::hash_combine(mMaterialPsoHash, mDesc->mBillboard);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mCullMode);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mZRead);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mZWrite);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mAlphaBlend);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mSrcBlend);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mDstBlend);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mAlphaTest);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mAlphaTestLess);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mAlphaTestVal);
	GHHash::hash_combine(mMaterialPsoHash, mDesc->mWireframe);
}
