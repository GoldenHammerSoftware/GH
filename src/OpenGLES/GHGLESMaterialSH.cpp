// Copyright 2010 Golden Hammer Software
#include "GHGLESMaterialSH.h"
#include "GHGLESShaderProgram.h"
#include "GHGLESInclude.h"
#include "GHBillboardTransformAdjuster.h"
#include "GHMath/GHTransform.h"
#include "GHViewInfo.h"
#include "GHGLESErrorReporter.h"
#include "GHTexture.h"
#include "GHGLESMaterialParamHandle.h"
#include <assert.h>
#include "GHMaterialCallbackType.h"
#include "GHMaterialCallback.h"
#include "GHUtils/GHEventMgr.h"
#include "GHRenderProperties.h"
#include "GHUtils/GHProfiler.h"
#include "GHGLESStateCache.h"

static const int kInvalidHandle = -1;

GHGLESMaterialSH::GHGLESMaterialSH(GHGLESShaderProgram* shader, GHMDesc* desc, GHEventMgr& eventMgr, GHGLESStateCache& stateCache)
: mDesc(desc)
, mShader(shader)
, mDeviceListener(eventMgr, *this)
, mQualityChangedListener(eventMgr, *this)
, mEventMgr(eventMgr)
, mStateCache(stateCache)
{
    assert(mShader);
    mShader->acquire();
    
    mTextureHandles.resize(mDesc->mTextures.size());
    attachTextureHandlesToShader();
    
    glUseProgram(mShader->getShaderProgram());
	mDescParamHandles = mDesc->initMaterial(*this);
}

GHGLESMaterialSH::~GHGLESMaterialSH(void)
{
    mShader->release();
	delete mDescParamHandles;
    delete mDesc;
}

void GHGLESMaterialSH::beginMaterial(const GHViewInfo& viewInfo)
{
	//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial", GHString::CHT_REFERENCE);

	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - doNothing", GHString::CHT_REFERENCE);
	}
	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - bindShader", GHString::CHT_REFERENCE);

		GHGLESErrorReporter::checkAndReportError("GHGLESMaterialSh::beginMaterial");
		mStateCache.setShader(mShader->getShaderProgram());
	}
    
	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - applyDefaultArgs", GHString::CHT_REFERENCE);

		// if the shader is shared between materials we need to
		// re-apply the default arguments that do not have callbacks.
		mDesc->applyDefaultArgs(*this, *mDescParamHandles);
	}

	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - applyStates", GHString::CHT_REFERENCE);

		applyAlpha();
		applyZBuffer();

		// We flip the y in the projection transform if we are render to texture in order to match d3d results.
		// This messes up the culling unless we flip the winding.
		applyCulling(viewInfo.isRenderToTexture());
	}


	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - applyTextures", GHString::CHT_REFERENCE);
		applyTextures();
	}

	{
		//GHPROFILESCOPE("GHGLESMaterialSH::beginMaterial - applyCallbacks", GHString::CHT_REFERENCE);

		// apply the callbacks after the above stuff in case a callback overrides a base value (like a texture).
		applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
	}
}

void GHGLESMaterialSH::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(geoData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbVal);
}

void GHGLESMaterialSH::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(entData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbVal);
}

void GHGLESMaterialSH::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
    GHBillboardTransformAdjuster transAdjuster;
    GHTransform scratchTransform;
    const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);
    
    GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
}

GHMaterialParamHandle* GHGLESMaterialSH::getParamHandle(const char* paramName)
{
    const char* alias = mDesc->checkParamAlias(paramName);
	int argumentHandle = mShader->getUniformLocation(alias);
	// check ==-1 and not <0 because we want to create a param handle for params that exist in the default quality.
	if (argumentHandle == -1)
	{
		return 0;
	}

	int size = mShader->getUniformSize(alias);

    return new GHGLESMaterialParamHandle(mEventMgr, mShader, alias, argumentHandle, size, *this);
}

static int calcGLBlend(GHMDesc::BlendMode mode)
{
    if (mode == GHMDesc::BM_ALPHA) return GL_SRC_ALPHA;
    if (mode == GHMDesc::BM_INVALPHA) return GL_ONE_MINUS_SRC_ALPHA;
    return GL_ONE;
}

void GHGLESMaterialSH::applyAlpha(void)
{
	if (mDesc->mAlphaBlend) 
	{
		mStateCache.setAlphaEnable(true);
		mStateCache.setAlphaFunc(calcGLBlend(mDesc->mSrcBlend), calcGLBlend(mDesc->mDstBlend));
		mIsAlpha = true;
	}
	else {
		mStateCache.setAlphaEnable(false);
		GHGLESErrorReporter::checkAndReportError("disable alpha blend");
	}
    //glDisable(GL_ALPHA_TEST);
}

void GHGLESMaterialSH::applyZBuffer(void)
{
	// from the docs for : Even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled.
	if (mDesc->mZWrite) {
		mStateCache.setZWrite(true);
		GHGLESErrorReporter::checkAndReportError("enableZWrite");
	}
	else {
		mStateCache.setZWrite(false);
		GHGLESErrorReporter::checkAndReportError("disableZRead");
	}
	if (mDesc->mZRead) {
		mStateCache.setZRead(true);
		GHGLESErrorReporter::checkAndReportError("enableZRead");
	}
	else {
		mStateCache.setZRead(false);
		GHGLESErrorReporter::checkAndReportError("disableZRead");
	}
}

void GHGLESMaterialSH::applyCulling(bool flipWinding)
{
	if (mDesc->mCullMode == GHMDesc::CM_NOCULL) 
	{
		mStateCache.setCullEnabled(false);
	}
	else 
	{
		mStateCache.setCullEnabled(true);

		if (flipWinding)
		{
			if (mDesc->mCullMode == GHMDesc::CM_CCW) {
				mStateCache.setCullFace(GL_BACK);
			}
			else {
				mStateCache.setCullFace(GL_FRONT);
			}
		}
		else
		{
			if (mDesc->mCullMode == GHMDesc::CM_CCW) {
				mStateCache.setCullFace(GL_FRONT);
			}
			else {
				mStateCache.setCullFace(GL_BACK);
			}
		}
	}
	if (mDesc->mZOffset > 0) {
		mStateCache.setZOffset(true);
	}
	else {
		mStateCache.setZOffset(false);
	}
	GHGLESErrorReporter::checkAndReportError("glCullFace");
}

void GHGLESMaterialSH::applyTextures(void)
{
    for (int i = 0; i < mDesc->mTextures.size(); ++i)
    {
        if (mTextureHandles[i] == kInvalidHandle || !mDesc->mTextures[i]->getTexture())
        {
            continue;
        }
		// I'm not quite sure what this line is.
		// We're saying the i'th texture is mTextureHandles[i],
		// but then we active texture GL_TEXTURE0+i and bind our info to that.
		// Maybe this is supposed to be GL_TEXTURE0+i as well?
		mStateCache.setTextureHandle(i, mTextureHandles[i]);
        
		mStateCache.setActiveTexture(i);
        mDesc->mTextures[i]->getTexture()->bind();
        
		mStateCache.setTextureWrap(mDesc->mTextures[i]->getWrapMode() == GHMDesc::WM_WRAP);
    }
}

void GHGLESMaterialSH::attachTextureHandlesToShader(void)
{
    for (int i = 0; i < mDesc->mTextures.size(); ++i)
    {
		mTextureHandles[i] = mShader->getUniformLocation(mDesc->mTextures[i]->getHandle());
    }
}

void GHGLESMaterialSH::onDeviceReinit(void)
{
    attachTextureHandlesToShader();
    glUseProgram(mShader->getShaderProgram());
    mDesc->applyDefaultArgs(*this, *mDescParamHandles);
}

void GHGLESMaterialSH::handleGraphicsQualityChanged(void)
{
	// for the moment same as reinit
	onDeviceReinit();
}

int GHGLESMaterialSH::getTextureIndex(int shaderHandle)
{
    // note: param alias seems to be bugged here.
    
    for (int i = 0; i < mAdditionalTextures.size(); ++i) {
        if (mAdditionalTextures[i] == shaderHandle) {
            return i + mDesc->mTextures.size();
        }
    }
    mAdditionalTextures.push_back(shaderHandle);
    return mAdditionalTextures.size() - 1 + mDesc->mTextures.size();
}

bool GHGLESMaterialSH::getExpectsDXLayout(void) const
{
	if (!mShader) return false;
	return mShader->getExpectsDXLayout();
}
