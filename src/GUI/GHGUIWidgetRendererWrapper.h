// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidgetRenderer.h"

// a widget renderer for wrapping a generic renderable.
class GHGUIWidgetRendererWrapper : public GHGUIWidgetRenderer
{
public:
    GHGUIWidgetRendererWrapper(GHRenderable* target, bool ownsTarget, float drawLayer);
    ~GHGUIWidgetRendererWrapper(void);

    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);

    // this class ignores gui rect info.
    virtual void updateRect(const GHRect<float,2>& rect, float angleRadians) {}
    virtual void updateDrawOrder(float baseGUIDrawOrder, float offset);
    
private:
    GHRenderable* mTarget;
    bool mOwnsTarget;
};
