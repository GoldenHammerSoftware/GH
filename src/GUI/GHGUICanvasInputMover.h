#pragma once

#include "GHUtils/GHController.h"
#include "GHInputObjectMover.h"
#include "GHMath/GHTransform.h"

class GHGUIWidget;
class IGHGUICanvas;

class GHGUICanvasInputMover : public GHController
{
public:
	GHGUICanvasInputMover(const GHInputState& inputState,
						  const GHTimeVal& timeVal,
						  const GHTransform& frameOfReference,
						  GHGUIWidget& widget);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	const GHInputState& mInputState;
	GHTransform			mTransform;
	GHInputObjectMover	mInputTransformMover;
	GHGUIWidget&		mWidget;
	IGHGUICanvas*		mCanvas{0};
};
