#pragma once

#include "GHUtils/GHController.h"
#include "GHMath/GHTransform.h"

class GHInputState;
class GHGUIWidget;

class GHGUIPoseableCanvasController : public GHController
{
public:
	GHGUIPoseableCanvasController(const GHInputState& inputState, 
								  GHGUIWidget& widget, 
								  unsigned int gamepadIndex, unsigned int poseableIndex,
								  const GHTransform& offsetTransform,
								  bool debugMove);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

private:
	GHTransform mOffsetTransform;
	const GHInputState& mInputState;
	GHGUIWidget& mWidget;
	unsigned int mGamepadIndex;
	unsigned int mPoseableIndex;
	bool mDebugMove;
};
