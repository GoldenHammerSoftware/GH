// Copyright Golden Hammer Software
#include "GHMDesc.h"
#include "GHTexture.h"
#include "GHMaterial.h"
#include "GHMaterialParamHandle.h"
#include "GHPlatform/GHDebugMessage.h"

GHMDesc::TextureInfo::TextureInfo(const char* handle, GHTexture* texture, WrapMode wrapMode)
: mTexture(0)
, mWrapMode(wrapMode)
, mHandle(handle, GHString::CHT_COPY)
{
    GHRefCounted::changePointer((GHRefCounted*&)mTexture, texture);
}

GHMDesc::TextureInfo::TextureInfo(const TextureInfo& other)
: mTexture(0)
, mWrapMode(other.mWrapMode)
, mHandle(other.mHandle)
{
    GHRefCounted::changePointer((GHRefCounted*&)mTexture, other.mTexture);
}

GHMDesc::TextureInfo::~TextureInfo(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mTexture, 0);
}

GHMDesc::FloatArg::FloatArg(const char* handle, unsigned int count, float* values)
: mCount(count)
, mValues(values)
, mHandle(handle, GHString::CHT_COPY)
{
}

GHMDesc::FloatArg::FloatArg(const FloatArg& other)
: mCount(other.mCount)
, mHandle(other.mHandle)
{
    mValues = new float[mCount];
    for (size_t i = 0; i < mCount; ++i)
    {
        mValues[i] = other.mValues[i];
    }
}

GHMDesc::FloatArg::~FloatArg(void)
{
    delete [] mValues;
}

GHMDesc::ParamHandles::~ParamHandles(void)
{
	for (size_t i = 0; i < mTextureParamHandles.size(); ++i)
	{
		delete mTextureParamHandles[i];
	}
	for (size_t i = 0; i < mFloatParamHandles.size(); ++i)
	{
		delete mFloatParamHandles[i];
	}
}

GHMDesc::GHMDesc(void)
: mBillboard(BT_NONE)
, mCullMode(CM_NOCULL)
, mSrcBlend(BM_ALPHA)
, mDstBlend(BM_INVALPHA)
, mDrawOrder(0)
, mZRead(true)
, mZWrite(true)
, mZOffset(0)
, mAlphaBlend(false)
, mAlphaTest(false)
, mAlphaTestLess(false)
, mAlphaTestVal(0.5)
, mWireframe(false)
{
    mPassMembership.setEmptySupportsAll(true);
}

GHMDesc::GHMDesc(const GHMDesc& other)
	: mVertexFile(other.mVertexFile)
	, mPixelFile(other.mPixelFile)
	, mBillboard(other.mBillboard)
	, mCullMode(other.mCullMode)
	, mDrawOrder(other.mDrawOrder)
	, mZRead(other.mZRead)
	, mZWrite(other.mZWrite)
	, mZOffset(other.mZOffset)
	, mAlphaBlend(other.mAlphaBlend)
	, mSrcBlend(other.mSrcBlend)
	, mDstBlend(other.mDstBlend)
	, mAlphaTest(other.mAlphaTest)
	, mAlphaTestLess(other.mAlphaTestLess)
	, mAlphaTestVal(other.mAlphaTestVal)
	, mWireframe(other.mWireframe)
{
	mParamAliases = other.mParamAliases;
	for (size_t texId = 0; texId < other.mTextures.size(); ++texId)
	{
		TextureInfo* copyTex = new TextureInfo(*other.mTextures[texId]);
		mTextures.push_back(copyTex);
	}
	for (size_t floatId = 0; floatId < other.mFloatArgs.size(); ++floatId)
	{
		FloatArg* copyArg = new FloatArg(*other.mFloatArgs[floatId]);
		mFloatArgs.push_back(copyArg);
	}
}

GHMDesc::~GHMDesc(void)
{
    for (size_t i = 0; i < mTextures.size(); ++i)
    {
        delete mTextures[i];
    }
    for (size_t i = 0; i < mFloatArgs.size(); ++i)
    {
        delete mFloatArgs[i];
    }
    PassOverrideList::const_iterator poIter;
    for (poIter = mPassOverrides.begin(); poIter != mPassOverrides.end(); ++poIter)
    {
        poIter->second->release();
    }

	for (LODList::iterator lodIter = mLODs.begin(); lodIter != mLODs.end(); ++lodIter)
	{
		lodIter->first->release();
	}
}

GHMDesc::ParamHandles* GHMDesc::initMaterial(GHMaterial& mat) const
{
	GHMDesc::ParamHandles* ret = new GHMDesc::ParamHandles;

    mat.setDrawOrder(mDrawOrder);
    mat.setIsAlpha(mAlphaBlend);
    mat.getPassMembership() = mPassMembership;
    
    GHMDesc::PassOverrideList::const_iterator poIter;
    for (poIter = mPassOverrides.begin(); poIter != mPassOverrides.end(); ++poIter)
    {
        mat.addPassOverride(poIter->first, poIter->second);
    }
    
	for (LODList::const_iterator lodIter = mLODs.begin(); lodIter != mLODs.end(); ++lodIter)
	{
		mat.addLOD(lodIter->first, lodIter->second);
	}

	// it is a code smell that we have textures and float args separate.
	for (unsigned int i = 0; i < mTextures.size(); ++i)
	{
		GHMaterialParamHandle* param = mat.getParamHandle(mTextures[i]->getHandle());
		// it is ok if this param is 0, we still want 0 in the list to keep the ordering correct.
		ret->mTextureParamHandles.push_back(param);
	}
	for (unsigned int i = 0; i < mFloatArgs.size(); ++i)
	{
		// it is ok if this param is 0, we still want 0 in the list to keep the ordering correct.
		GHMaterialParamHandle* param = mat.getParamHandle(mFloatArgs[i]->getHandle());
		ret->mFloatParamHandles.push_back(param);
	}

    applyDefaultArgs(mat, *ret);

	return ret;
}

void GHMDesc::applyTextures(GHMaterial& mat, const GHMDesc::ParamHandles& handles, bool isOverrideMat) const
{
	assert(handles.mTextureParamHandles.size() == mTextures.size());

	for (unsigned int i = 0; i < mTextures.size(); ++i)
	{
		GHMaterialParamHandle* param = handles.mTextureParamHandles[i];
		if (param)
		{
			GHMaterialParamHandle::TextureHandle texHandle(mTextures[i]->getTexture(), mTextures[i]->getWrapMode());
			param->setValue(GHMaterialParamHandle::HT_TEXTURE, &texHandle);
		}
        /* this is super spammy in the transitional metal
		else
		{
			if (!isOverrideMat)
			{
				GHDebugMessage::outputString("GHMDesc::applyTextures could not find param handle for %s", mTextures[i]->getHandle());
			}
		}
        */
	}
}

void GHMDesc::applyDefaultArgs(GHMaterial& mat, const GHMDesc::ParamHandles& handles) const
{
	assert(handles.mFloatParamHandles.size() == mFloatArgs.size());

	for (unsigned int i = 0; i < mFloatArgs.size(); ++i)
	{
		GHMaterialParamHandle* param = handles.mFloatParamHandles[i];
		if (param)
		{
			//todo: support arrays
			GHMaterialParamHandle::HandleType handleType = GHMaterialParamHandle::calcHandleType(mFloatArgs[i]->getCount()*sizeof(float));
			param->setValue(handleType, mFloatArgs[i]->getValues());
		}
	}
	// do we want to apply textures here instead of having them called separately?
}

const char* GHMDesc::checkParamAlias(const char* paramName)
{
	std::map<GHString, GHString>::const_iterator alias;
	alias = mParamAliases.find(GHString(paramName, GHString::CHT_REFERENCE));
	if (alias != mParamAliases.end()) {
		return alias->second.getChars();
	}
	return paramName;
}

