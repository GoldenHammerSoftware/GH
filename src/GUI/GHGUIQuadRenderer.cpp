// Copyright Golden Hammer Software
#include "GHGUIQuadRenderer.h"
#include "GHVBDescription.h"
#include "GHVertexBuffer.h"
#include "GHVBFactory.h"
#include "GHRenderPass.h"
#include "GHIndexBufferUtil.h"
#include "GHVertexBufferUtil.h"
#include "GHMaterial.h"
#include "GHGUIWidget.h"
#include "GHScreenInfo.h"

GHGUIQuadRenderer::GHGUIQuadRenderer(const GHScreenInfo& screenInfo,
                                     const GHVBFactory& vbFactory, GHMaterial* material,
                                     const GHPoint2& minUV, const GHPoint2& maxUV,
                                     float drawOrderOffset, float drawLayer,
									 const GHPoint2i& gridSize,
									 float uvAngleInRadians)
: GHGUIWidgetRenderer(drawLayer, drawOrderOffset)
, mScreenInfo(screenInfo)
, mMaterial(0)
, mVB(0)
, mGridSize(gridSize)
, mUVAngleInRadians(uvAngleInRadians)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, material);

	mUV.mMin.setCoords(minUV.getCoords());
	mUV.mMax.setCoords(maxUV.getCoords());

	GHVBDescription vbdesc((1 + mGridSize[0])*(1 + mGridSize[1]), 6 * mGridSize[0]*mGridSize[1], GHVBBlitter::BT_INDEX);
    GHVBDescription::StreamDescription streamDesc;
    streamDesc.mIsShared = true;
    streamDesc.mUsage = GHVBUsage::DYNAMIC;
    streamDesc.mComps.push_back(GHVertexComponent::AP_POS2D);
    streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
    vbdesc.addStreamDescription(streamDesc);
    mVB = vbFactory.createVB(vbdesc);

	GHVertexBufferUtil::createUVGrid(*mVB, minUV, maxUV, mGridSize[0], mGridSize[1], uvAngleInRadians);
    GHRect<float,2> rect;
    rect.mMin.setCoords(0,0);
    rect.mMax.setCoords(1,1);
    updateRect(rect, 0);
    
    GHVBBlitterPtr* blitterPtr =  mVB->getBlitter();
    GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();
	GHIndexBufferUtil::fillIndexBufferForGrid(*indexBlitter, mGridSize[0] + 1, mGridSize[1] + 1);
}

GHGUIQuadRenderer::~GHGUIQuadRenderer(void)
{
    GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
    delete mVB;
}

void GHGUIQuadRenderer::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
    if (!mVB || !mMaterial) {
        return;
    }
	const GHPropertyContainer* props = 0;
	if (mWidget)
	{
		props = &mWidget->getPropertyContainer();
	}
	pass.getRenderList().setEntityData(props);
    pass.getRenderList().addEntry(mVB, mMaterial, 0, 0, mDrawOrder);
}

void GHGUIQuadRenderer::updateRect(const GHRect<float,2>& rect, float angleRadians)
{
    GHVertexStreamPtr* streamPtr = mVB->getStream(0);
    GHVertexStream* stream = streamPtr->get();
    const GHVertexStreamFormat& streamFormat = stream->getFormat();
   
	GHRect<float, 2> clampedRect;
	GHRect<float, 2> clampedUV;
	GHVertexBufferUtil::clampToScreenEdges(rect, mUV, clampedRect, clampedUV);

	GHVertexBufferUtil::createUVGrid(*mVB, clampedUV.mMin, clampedUV.mMax, mGridSize[0], mGridSize[1], mUVAngleInRadians);

    float* streamVerts = stream->lockWriteBuffer();
	GHVertexBufferUtil::fillGrid2D(streamVerts, streamFormat, clampedRect.mMin, clampedRect.mMax, mGridSize[0], mGridSize[1], angleRadians, mScreenInfo.getAspectRatio());
    stream->unlockWriteBuffer();
}

