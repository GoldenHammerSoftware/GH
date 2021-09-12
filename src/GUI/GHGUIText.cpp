// Copyright Golden Hammer Software
#include "GHGUIText.h"
#include "GHFontRenderer.h"
#include "GHGUIRenderable.h"
#include "GHGUIRectMaker.h"
#include "GHGeometry.h"
#include "GHFont.h"
#include "GHGUIWidgetRendererWrapper.h"
#include <string.h> //for strcmp
#include "GHGUICanvas.h"

GHGUIText::GHGUIText(const GHFontRenderer& fontRenderer,
                     GHGUIRenderable& renderer,
                     const GHGUIRectMaker& rectMaker,
                     GHFont* font,
                     const char* text, float textHeight, bool wrap, GHFillType::Enum fill,
                     GHAlign::Enum hAlign, GHAlign::Enum vAlign,
                     float drawLayer)
: GHGUIWidget(rectMaker)
, mFontRenderer(fontRenderer)
, mGUIRenderable(renderer)
, mRectMaker(rectMaker)
, mString(text, GHString::CHT_COPY)
, mFont(0)
, mTextHeight(textHeight)
, mFill(fill)
, mHAlign(hAlign)
, mVAlign(vAlign)
, mWrap(wrap)
{
    GHRefCounted::changePointer((GHRefCounted*&)mFont, font);
    mGeometry = new GHGeometry();
    mGeometry->setMaterial(font->getMaterial());
    mRenderable.addGeometry(mGeometry);
	mRenderable.setEntityData(&mPropertyContainer);
    
    mWidgetRenderer = new GHGUIWidgetRendererWrapper(&mRenderable, false, drawLayer);
	mWidgetRenderer->setWidget(this);
}

GHGUIText::~GHGUIText(void)
{
    // mRenderable owns mGeometry.
    GHRefCounted::changePointer((GHRefCounted*&)mFont, 0);
    delete mWidgetRenderer;
}

void GHGUIText::onEnterTransitionIn(void)
{
    mGUIRenderable.addRenderable(*mWidgetRenderer);
    updatePosition();
}

void GHGUIText::onFinishTransitionOut(void)
{
    mGUIRenderable.removeRenderable(*mWidgetRenderer);
}

void GHGUIText::setText(const char* text)
{
    if (!::strcmp(text, mString.getChars())) return;
    mString.setConstChars(text, GHString::CHT_COPY);
    updateRenderedText();
}

void GHGUIText::setAlignment(GHAlign::Enum hAlign, GHAlign::Enum vAlign)
{
    mHAlign = hAlign;
    mVAlign = vAlign;
}

void GHGUIText::setWrap(bool wrap)
{
	mWrap = wrap;
}

void GHGUIText::updateRenderedText(void)
{
    if (!mFont) return;
    
    GHVertexBuffer* newVB = 0;
    IGHGUICanvas* canvas = IGHGUICanvas::get3dCanvas(getPropertyContainer());
	if (canvas && !canvas->is2d())
	{
		if (canvas->getScale()[0] != canvas->getScale()[1])
		{
			int brkpt = 1;
		}
	}
	float pixelSizeMod = calcPixelSizeMod();
    float screenTextHeight = mRectMaker.createScreenPct(mTextHeight, mFill, 1, &mScreenPos, pixelSizeMod, canvas);
    
    GHVertexBufferPtr* oldVBPtr = mGeometry->getVB();
    GHVertexBuffer* oldVB = 0;
    if (oldVBPtr) oldVB = oldVBPtr->get();
    
	GHGUIRectMaker::CanvasInfo canvasInfo = mRectMaker.calcCanvasInfo(canvas, pixelSizeMod);
	float aspectRatio = (float)canvasInfo.mCanvasSize[0] / (float)canvasInfo.mCanvasSize[1];

    newVB = mFontRenderer.createText(*mFont, mString.getChars(), screenTextHeight, aspectRatio,
                                     mWrap, mVAlign, mHAlign,
                                     mScreenPos.mMin, mScreenPos.mMax,
                                     oldVB);
    if (newVB && newVB != oldVB)
    {
        mGeometry->setVB(new GHVertexBufferPtr(newVB));
    }
	else if (!newVB)
	{
		mGeometry->setVB(0);
	}
}

void GHGUIText::updatePosition(void)
{
    GHGUIWidget::updatePosition();
    updateRenderedText();
}

