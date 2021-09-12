#include "GHGUIPoseableCanvasController.h"
#include "GHInputState.h"
#include "GHGUIWidget.h"
#include "GHGUICanvas.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHMath/GHMathDebugPrint.h"

class Foo
{
public:
	void func(bool b);
};

void Foo::func(const bool b)
{

}

GHGUIPoseableCanvasController::GHGUIPoseableCanvasController(const GHInputState& inputState, 
															 GHGUIWidget& widget, 
															 unsigned int gamepadIndex, unsigned int poseableIndex,
															 const GHTransform& offsetTransform,
															 bool debugMove)
	: mOffsetTransform(offsetTransform)
	, mInputState(inputState)
	, mWidget(widget)
	, mGamepadIndex(gamepadIndex)
	, mPoseableIndex(poseableIndex)
	, mDebugMove(debugMove)
{

}

void GHGUIPoseableCanvasController::onActivate(void)
{

}

void GHGUIPoseableCanvasController::onDeactivate(void)
{

}

void debugOnly_manuallyMoveOffset(const GHInputState& inputState, GHTransform& transform)
{
	//inputState.getKeyState(GHKeyDef::key_gp_)

	auto& gamepad = inputState.getGamepad(1);

	GHPoint3 rot(0,0,0), pos(0, 0, 0);
	{
		rot[0] = gamepad.mJoySticks[1][0];
		rot[2] = gamepad.mJoySticks[1][1];
		rot[1] = gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTBUMPER) - gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_RIGHTTRIGGER);
	}

	{
		pos[0] = gamepad.mJoySticks[0][0];
		pos[2] = gamepad.mJoySticks[0][1];
		pos[1] = gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTBUMPER) - gamepad.mButtons.getKeyState(GHKeyDef::KEY_GP_LEFTTRIGGER);
	}

	if (rot.lenSqr() || pos.lenSqr())
	{
		rot *= .05f;
		pos *= .01f;

		transform.multDir(pos, pos);

		GHTransform offset;
		offset.becomeYawPitchRollRotation(rot[0], rot[1], rot[2]);
		offset.setTranslate(pos);

		offset.mult(transform, transform);

		//transform.debugPrint("GHTransform transform(", ", ", ");");
		GHMathDebugPrint::printPoint(transform.getMatrix(), "offsetTransform=\"", " ", "\"");
	}
}

void GHGUIPoseableCanvasController::update(void)
{
	auto& poseable = mInputState.getGamepad(mGamepadIndex).mPoseables[mPoseableIndex];

	IGHGUICanvas* canvas = GHGUICanvas::get3dCanvas(mWidget.getPropertyContainer());
	if (!canvas)
	{
		GHDebugMessage::outputString("No canvas found on gui panel with GHGUIPoseableCanvasController. panel will not update");
		return;
	}

	if (mDebugMove)
	{
		debugOnly_manuallyMoveOffset(mInputState, mOffsetTransform);
	}

	GHTransform finalTransform;
	mOffsetTransform.mult(poseable.mPosition, finalTransform);

	GHPoint3 pos;
	finalTransform.getTranslate(pos);
	canvas->setPos(pos);
	
	GHPoint3 rot;
	finalTransform.calcYawPitchRoll(rot[0], rot[1], rot[2]);
	canvas->setRot(rot);
}

