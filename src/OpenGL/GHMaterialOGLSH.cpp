// Copyright 2010 Golden Hammer Software
#include "GHMaterialOGLSH.h"
#include "GHOGLShaderProgram.h"
#include "GHOGLInclude.h"
#include "GHBillboardTransformAdjuster.h"
#include "GHTransform.h"
#include "GHViewInfo.h"
#include "GHGLErrorReporter.h"
#include "GHTexture.h"
#include "GHMaterialParamHandleOGLSH.h"
#include <assert.h>
#include "GHMaterialCallback.h"

static const int kInvalidHandle = -1;

GHMaterialOGLSH::GHMaterialOGLSH(GHOGLShaderProgram* shader, GHMDesc* desc)
: mDesc(desc)
, mShader(shader)
{
    mTextureHandles.resize(mDesc->mTextures.size());
    for (int i = 0; i < mDesc->mTextures.size(); ++i)
    {
        mTextureHandles[i] = glGetUniformLocation(mShader->getShaderProgram(), mDesc->mTextures[i]->getHandle());
    }
    
    glUseProgram(mShader->getShaderProgram());
    mDesc->initMaterial(*this);
}

GHMaterialOGLSH::~GHMaterialOGLSH(void)
{
    delete mShader;
    delete mDesc;
}

void GHMaterialOGLSH::beginMaterial(const GHViewInfo& viewInfo)
{
	glUseProgram(mShader->getShaderProgram());

    applyCallbacks(GHMaterialCallbackType::CT_PERFRAME, 0);
    
    applyAlpha();
	applyZBuffer();
	applyCulling();
    applyTextures();
    applyWireframe();
}

void GHMaterialOGLSH::beginGeometry(const GHPropertyContainer* geoData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(geoData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERGEO, &cbVal);
}

void GHMaterialOGLSH::beginEntity(const GHPropertyContainer* entData, const GHViewInfo& viewInfo)
{
    GHMaterialCallback::CBVal_PerObj cbVal(entData, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERENT, &cbVal);
}

void GHMaterialOGLSH::beginTransform(const GHTransform& modelToWorld, const GHViewInfo& viewInfo)
{
    GHBillboardTransformAdjuster transAdjuster;
    GHTransform scratchTransform;
    const GHTransform& applyTrans = transAdjuster.adjustTransform(modelToWorld, mDesc->mBillboard, viewInfo, scratchTransform);
    
    GHMaterialCallback::CBVal_PerTrans cbArg(applyTrans, viewInfo);
    applyCallbacks(GHMaterialCallbackType::CT_PERTRANS, &cbArg);
}

GHMaterialParamHandle* GHMaterialOGLSH::getParamHandle(const char* paramName)
{
    int argumentHandle = glGetUniformLocation(mShader->getShaderProgram(), paramName);
	if (argumentHandle == -1)
	{
		return 0;
	}
    return new GHMaterialParamHandleOGLSH(argumentHandle, *this);
}

static int calcGLBlend(GHMDesc::BlendMode mode)
{
    if (mode == GHMDesc::BM_ALPHA) return GL_SRC_ALPHA;
    if (mode == GHMDesc::BM_INVALPHA) return GL_ONE_MINUS_SRC_ALPHA;
    return GL_ONE;
}

void GHMaterialOGLSH::applyAlpha(void)
{
	if (mDesc->mAlphaBlend) 
	{
		glEnable(GL_BLEND);
        glBlendFunc(calcGLBlend(mDesc->mSrcBlend), calcGLBlend(mDesc->mDstBlend));
		mIsAlpha = true;
	}
	else {
		glDisable(GL_BLEND);
		GHGLErrorReporter::checkAndReportError("disable alpha blend");
	}
    //glDisable(GL_ALPHA_TEST);
}

void GHMaterialOGLSH::applyZBuffer(void)
{
	// from the docs for : Even if the depth buffer exists and the depth mask is non-zero, the depth buffer is not updated if the depth test is disabled.
	if (mDesc->mZWrite) {
		glDepthMask(GL_TRUE);
		GHGLErrorReporter::checkAndReportError("enableZWrite");
	}
	else {
		glDepthMask(GL_FALSE);
		GHGLErrorReporter::checkAndReportError("disableZRead");
	}
	if (mDesc->mZRead) {
		glEnable(GL_DEPTH_TEST);
		GHGLErrorReporter::checkAndReportError("enableZRead");
	}
	else {
		glDisable(GL_DEPTH_TEST);
		GHGLErrorReporter::checkAndReportError("disableZRead");
	}
}

void GHMaterialOGLSH::applyCulling(void)
{
	if (mDesc->mCullMode == GHMDesc::CM_NOCULL) {
		glDisable(GL_CULL_FACE);
	}
	else {
		glEnable(GL_CULL_FACE);
		if (mDesc->mCullMode == GHMDesc::CM_CCW) {
			glCullFace(GL_FRONT);
		}
		else {
			glCullFace(GL_BACK);
		}
	}
	if (mDesc->mZOffset > 0) {
		glDepthRange(0, 1);
	}
	else {
		glDepthRange(0.1, 1);
	}
	GHGLErrorReporter::checkAndReportError("glCullFace");
}

void GHMaterialOGLSH::applyTextures(void)
{
    for (int i = 0; i < mDesc->mTextures.size(); ++i)
    {
        if (mTextureHandles[i] == kInvalidHandle || !mDesc->mTextures[i]->getTexture())
        {
            continue;
        }
        glUniform1i(mTextureHandles[i], i);
        
        int texIdx = GL_TEXTURE0 + i;
        glActiveTexture(texIdx);
        mDesc->mTextures[i]->getTexture()->bind();
        
        applyTextureWrap(i, mDesc->mTextures[i]->getWrapMode());
    }
}

void GHMaterialOGLSH::applyTextureWrap(int textureStage, GHMDesc::WrapMode val)
{
	if (val == GHMDesc::WM_WRAP)
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	}
	else
	{
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE );
		glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE );
	}
}

void GHMaterialOGLSH::applyWireframe(void)
{
    if (mDesc->mWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    } else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
}

int GHMaterialOGLSH::getTextureIndex(int shaderHandle)
{
    for (int i = 0; i < mAdditionalTextures.size(); ++i) {
        if (mAdditionalTextures[i] == shaderHandle) {
            return i + mDesc->mTextures.size();
        }
    }
    mAdditionalTextures.push_back(shaderHandle);
    return mAdditionalTextures.size() + mDesc->mTextures.size();
}



