#include "GHGUIWidgetRendererModel.h"
#include "GHModel.h"
#include "GHRenderPass.h"
#include "GHGeometryRenderable.h"
#include "GHGUIWidget.h"
#include "GHGUICanvas.h"

GHGUIWidgetRendererModel::GHGUIWidgetRendererModel(GHModel* model, const GHPoint3& pos, const GHPoint3& scale)
	: mModel(model)
	, mPos(pos)
	, mScale(scale)
{
	mModel->acquire();
}

GHGUIWidgetRendererModel::~GHGUIWidgetRendererModel(void)
{
	mModel->release();
}

void GHGUIWidgetRendererModel::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
	mModel->getRenderable()->collect(pass, frustum);
}

void GHGUIWidgetRendererModel::updateRect(const GHRect<float, 2>& rect, float angleRadians)
{
	//Todo: handle angleRadians

	IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(mWidget->getPropertyContainer());
	if (canvas)
	{
		auto& modelTrans = mModel->getSkeleton()->getLocalTransform();

		GHPoint3 pos = canvas->getPos();
		const GHPoint3& rot = canvas->getRot();
		const GHPoint3& scale = canvas->getScale();

		modelTrans.becomeYawPitchRollRotation(rot[0], rot[1], rot[2]);
		GHTransform scaleTrans;
		
		scaleTrans.becomeScaleMatrix(scale[0], scale[1], scale[2]);
		modelTrans.mult(scaleTrans, modelTrans);

		GHPoint3 ourPosition;
		modelTrans.multDir(mPos, ourPosition);
		pos += ourPosition;

		scaleTrans.becomeScaleMatrix(mScale[0], mScale[1], mScale[2]);
		scaleTrans.mult(modelTrans, modelTrans);

		modelTrans.setTranslate(pos);
	}
}

void GHGUIWidgetRendererModel::updateDrawOrder(float baseGUIDrawOrder, float offset)
{
	GHGUIWidgetRenderer::updateDrawOrder(baseGUIDrawOrder, offset);
	mModel->getRenderable()->setDrawOrder(baseGUIDrawOrder + offset);
}

