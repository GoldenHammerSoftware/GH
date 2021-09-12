// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidget.h"

class GHGUIRenderable;
class GHGUIWidgetRenderer;

class GHGUIPanel : public GHGUIWidget
{
public:
    GHGUIPanel(GHGUIRenderable& renderer, const GHGUIRectMaker& rectMaker);
    ~GHGUIPanel(void);

    virtual void updatePosition(void);
    
    void setWidgetRenderer(GHGUIWidgetRenderer* renderer);
    
protected:
    virtual void onEnterTransitionIn(void);
    virtual void onFinishTransitionIn(void) {}
    virtual void onEnterTransitionOut(void) {}
    virtual void onFinishTransitionOut(void);

protected:
    GHGUIRenderable& mRenderer;
    // optional renderable
    GHGUIWidgetRenderer* mWidgetRenderer;
};
