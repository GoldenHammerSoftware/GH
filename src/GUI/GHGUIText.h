// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidget.h"
#include "Render/GHGeometryRenderable.h"
#include "GHString/GHString.h"
#include "GHFillType.h"
#include "GHUtils/GHAlign.h"

class GHFontRenderer;
class GHGUIRenderable;
class GHGUIRectMaker;
class GHFont;
class GHGUIWidgetRendererWrapper;

// a gui widget that wraps a displayed piece of text.
class GHGUIText : public GHGUIWidget
{
public:
    GHGUIText(const GHFontRenderer& fontRenderer,
              GHGUIRenderable& renderer,
              const GHGUIRectMaker& rectMaker,
              GHFont* font,
              const char* text, float textHeight, bool wrap, GHFillType::Enum fill,
              GHAlign::Enum hAlign, GHAlign::Enum vAlign,
              float drawLayer);
    ~GHGUIText(void);
    
    virtual void updatePosition(void);
    
    void setText(const char* text);
    const char* getText(void) const { return mString.getChars(); }
    
    void setTextHeight(float textHeight) { mTextHeight = textHeight; }
    void setHeightFillType(GHFillType::Enum fill) { mFill = fill; }
    void setAlignment(GHAlign::Enum hAlign, GHAlign::Enum vAlign);
	void setWrap(bool wrap);

	float getTextHeight(void) const { return mTextHeight; }
	GHFillType::Enum getHeightFillType(void) const { return mFill; }
	GHAlign::Enum getHAlign(void) const { return mHAlign; }
	GHAlign::Enum getVAlign(void) const { return mVAlign; }
	bool getWrap(void) const { return mWrap; }
    
protected:
    virtual void onEnterTransitionIn(void);
    virtual void onFinishTransitionIn(void) {}
    virtual void onEnterTransitionOut(void) {}
    virtual void onFinishTransitionOut(void);
    
private:
    void updateRenderedText(void);
    
private:
    const GHFontRenderer& mFontRenderer;
    GHGUIRenderable& mGUIRenderable;
    const GHGUIRectMaker& mRectMaker;
  
    GHFont* mFont;
    GHString mString;
    float mTextHeight;
	bool mWrap;
    GHFillType::Enum mFill;
    GHAlign::Enum mHAlign;
    GHAlign::Enum mVAlign;
    
    // we push our vb and material into a geometry
    GHGeometry* mGeometry;
    // and push that into a renderable so it can be drawn through the scene.
    GHGeometryRenderable mRenderable;
    GHGUIWidgetRendererWrapper* mWidgetRenderer;
};
