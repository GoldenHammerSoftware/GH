// Copyright Golden Hammer Software
#include "GHGUIProjectedLineRenderer.h"
#include "GHMaterial.h"
#include "GHRenderPass.h"
#include "GHIndexBufferUtil.h"
#include "GHVertexBufferUtil.h"
#include "GHVBFactory.h"
#include "GHVertexBuffer.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIWidget.h"

GHGUIProjectedLineRenderer::GHGUIProjectedLineRenderer(GHMaterial* material,
													   const GHViewInfo& viewInfo,
													   float thickness,
													   float drawOrderOffset, float drawLayer)
: GHGUIWidgetRenderer(drawLayer, drawOrderOffset)
, mMaterial(0)
, mVB(0)
, mThickness(thickness)
, mUnprojector(viewInfo)
{
	GHRefCounted::changePointer((GHRefCounted*&)mMaterial, material);
}

GHGUIProjectedLineRenderer::~GHGUIProjectedLineRenderer(void)
{
	GHRefCounted::changePointer((GHRefCounted*&)mMaterial, 0);
	delete mVB;
}

void GHGUIProjectedLineRenderer::addPoint(const GHPoint3& point)
{
	mPoints.push_back(point);
}

void GHGUIProjectedLineRenderer::generateGeometry(const GHVBFactory& vbFactory)
{
	if (mPoints.size() < 2) {
		return;
	}

	unsigned int numIndices = (unsigned int)(mPoints.size() - 1) * 6;
	unsigned int numVerts = (unsigned int)mPoints.size() * 2;
	GHVBDescription vbDesc(numVerts, numIndices, GHVBBlitter::BT_INDEX);
	GHVBDescription::StreamDescription streamDesc;
	streamDesc.mIsShared = true;
	streamDesc.mUsage = GHVBUsage::DYNAMIC;
	streamDesc.mComps.push_back(GHVertexComponent::AP_POS2D);
	streamDesc.mComps.push_back(GHVertexComponent::AP_UV1);
	vbDesc.addStreamDescription(streamDesc);
	mVB = vbFactory.createVB(vbDesc);

	/* Fill the UV components of the vertices. May not be necessary.
	GHVertexStreamPtr* streamPtr = mVB->getStream(0);
	GHVertexStream* stream = streamPtr->get();
	const GHVertexStreamFormat& streamFormat = stream->getFormat();
	const GHVertexStreamFormat::ComponentEntry* uvEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_UV1);
	float* uv = stream->lockWriteBuffer() + uvEntry->mOffset;
	for (int i = 0; i < numVerts; ++i, uv += streamFormat.getVertexSize())
	{
		*uv = *(uv+1) = 1.f;
	}
	stream->unlockWriteBuffer();
	*/

	updatePositions();

	GHVBBlitterPtr* blitterPtr = mVB->getBlitter();
	GHVBBlitterIndex* indexBlitter = (GHVBBlitterIndex*)blitterPtr->get();

	int numQuads = (int)mPoints.size() - 1;
	GHIndexBufferUtil::fillIndexBufferForQuads(*indexBlitter, numQuads, true);
}

void GHGUIProjectedLineRenderer::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
	if (!mVB || !mMaterial) {
		return;
	}
	updatePositions();
	const GHPropertyContainer* props = 0;
	if (mWidget)
	{
		props = &mWidget->getPropertyContainer();
	}

	pass.getRenderList().addEntry(mVB, mMaterial, 0, props, mDrawOrder);
}

void GHGUIProjectedLineRenderer::updatePositions(void)
{
	if (!mVB) {
		return;
	}

	size_t numPoints = mPoints.size();
	if (numPoints < 2) {
		return;
	}

	GHVertexStreamPtr* streamPtr = mVB->getStream(0);
	GHVertexStream* stream = streamPtr->get();
	const GHVertexStreamFormat& streamFormat = stream->getFormat();
	const GHVertexStreamFormat::ComponentEntry* posCompEntry = streamFormat.getComponentEntry(GHVertexComponentShaderID::SI_POS);

	float* streamVerts = stream->lockWriteBuffer() + posCompEntry->mOffset;

	GHPoint2 prevPoint;
	mUnprojector.project(mPoints[0], prevPoint);

	for (size_t i = 1; i < numPoints; ++i)
	{
		GHPoint2 currPoint;
		mUnprojector.project(mPoints[i], currPoint);
	//	GHDebugMessage::outputString("%f %f %f -> %f %f", mPoints[i][0], mPoints[i][1], mPoints[i][2], currPoint[0], currPoint[1]);

		GHPoint2 diff = currPoint;
		diff -= prevPoint;
		diff.normalize();

		GHPoint2 perp(diff[1], -diff[0]);
		perp *= mThickness*.5f;

		//do the first set of points only on the first iteration
		if (i == 1)
		{
			GHPoint2 left = prevPoint, right = prevPoint;
			left -= perp;
			right += perp;

			*streamVerts = left[0];
			*(streamVerts + 1) = left[1];
			streamVerts += streamFormat.getVertexSize();
			*streamVerts = right[0];
			*(streamVerts + 1) = right[1];
			streamVerts += streamFormat.getVertexSize();
		}

		GHPoint2 left = currPoint, right = currPoint;
		left -= perp;
		right += perp;

		*streamVerts = left[0];
		*(streamVerts + 1) = left[1];
		streamVerts += streamFormat.getVertexSize();
		*streamVerts = right[0];
		*(streamVerts + 1) = right[1];
		streamVerts += streamFormat.getVertexSize();

		prevPoint = currPoint;
	}
	stream->unlockWriteBuffer();
}

void GHGUIProjectedLineRenderer::updateRect(const GHRect<float, 2>& rect)
{

}
