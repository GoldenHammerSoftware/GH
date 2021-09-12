// Copyright Golden Hammer Software
#include "GHGUIPanel.h"
#include "GHGUIRenderable.h"
#include "GHGUIWidgetRenderer.h"

GHGUIPanel::GHGUIPanel(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker)
: GHGUIWidget(rectMaker)
, mRenderer(renderer)
, mWidgetRenderer(0)
{
}

GHGUIPanel::~GHGUIPanel(void)
{
    delete mWidgetRenderer;
}

void GHGUIPanel::onEnterTransitionIn(void)
{
    if (mWidgetRenderer) {
        mRenderer.addRenderable(*mWidgetRenderer);
    }
    updatePosition();
}

void GHGUIPanel::onFinishTransitionOut(void)
{
    if (mWidgetRenderer) {
        mRenderer.removeRenderable(*mWidgetRenderer);
    }
}

void GHGUIPanel::updatePosition(void)
{
    GHGUIWidget::updatePosition();
    if (mWidgetRenderer) {
        mWidgetRenderer->updateRect(mScreenPos, mPosDesc.mAngleRadians);
    }
}

void GHGUIPanel::setWidgetRenderer(GHGUIWidgetRenderer* renderer)
{
	delete mWidgetRenderer;
    mWidgetRenderer = renderer;
}

