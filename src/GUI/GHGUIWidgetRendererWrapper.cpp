// Copyright Golden Hammer Software
#include "GHGUIWidgetRendererWrapper.h"
#include <assert.h>
#include "GHGUIWidget.h"

GHGUIWidgetRendererWrapper::GHGUIWidgetRendererWrapper(GHRenderable* target, bool ownsTarget, float drawLayer)
: GHGUIWidgetRenderer(drawLayer, 0)
, mTarget(target)
, mOwnsTarget(ownsTarget)
{
    assert(mTarget);
}

GHGUIWidgetRendererWrapper::~GHGUIWidgetRendererWrapper(void)
{
    if (mOwnsTarget) delete mTarget;
}

void GHGUIWidgetRendererWrapper::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
	if (mWidget)
	{
		mTarget->setEntityData(&mWidget->getPropertyContainer());
	}
    mTarget->collect(pass, frustum);
}

void GHGUIWidgetRendererWrapper::updateDrawOrder(float baseGUIDrawOrder, float offset)
{
	GHGUIWidgetRenderer::updateDrawOrder(baseGUIDrawOrder, offset);
    mTarget->setDrawOrder(mDrawOrder);
}
