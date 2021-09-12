#include "GHGUICanvasInputMover.h"
#include "GHGUICanvas.h"
#include "GHGUIWidget.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHKeyDef.h"
#include "GHInputState.h"

GHGUICanvasInputMover::GHGUICanvasInputMover(const GHInputState& inputState,
											 const GHTimeVal& timeVal,
											 const GHTransform& frameOfReference,
											 GHGUIWidget& widget)
	: mInputState(inputState)
	, mInputTransformMover(mTransform, inputState, timeVal)
	, mWidget(widget)
{
	//todo: set from xml?
	// default values:
	//(note: I don't care which axis is which or in which direction, but i do care which buttons are used)
	mInputTransformMover.setFrameOfReference(&frameOfReference);
	mInputTransformMover.addJoystickMoveIndex(0);
	mInputTransformMover.addJoystickRotateIndex(1);

	mInputTransformMover.addCommand(GHKeyDef::KEY_GP_LEFTBUMPER, GHInputObjectMover::OM_UP);
	mInputTransformMover.addCommand(GHKeyDef::KEY_GP_LEFTTRIGGER, GHInputObjectMover::OM_DOWN);
	mInputTransformMover.addCommand(GHKeyDef::KEY_GP_RIGHTBUMPER, GHInputObjectMover::OM_TURNLEFT);
	mInputTransformMover.addCommand(GHKeyDef::KEY_GP_RIGHTTRIGGER, GHInputObjectMover::OM_TURNRIGHT);
}

void GHGUICanvasInputMover::onActivate(void)
{
	mCanvas = GHGUICanvas::get3dCanvas(mWidget.getPropertyContainer());

	if (mCanvas)
	{
		const GHPoint3& pos = mCanvas->getPos();
		const GHPoint3& rot = mCanvas->getRot();
		const GHPoint3& scale = mCanvas->getScale();

		mTransform.becomeYawPitchRollRotation(rot[0], rot[1], rot[2]);
		GHTransform scaleTrans;
		scaleTrans.becomeScaleMatrix(scale[0], scale[1], scale[2]);

		scaleTrans.mult(mTransform, mTransform);

		mTransform.setTranslate(pos);
	}
	else
	{
		mTransform.becomeIdentity();
		GHDebugMessage::outputString("Error creating GHGUICanvasInputMover:onActivate: no canvas on widget");
	}

	mInputTransformMover.onActivate();
}

void GHGUICanvasInputMover::onDeactivate(void)
{
	mInputTransformMover.onDeactivate();
}

void GHGUICanvasInputMover::update(void)
{
	if (!mCanvas)
	{
		return;
	}

	mInputTransformMover.update();
	
	GHPoint3 pos;
	mTransform.getTranslate(pos);
	mCanvas->setPos(pos);

	GHPoint3 rot;
	mTransform.calcYawPitchRoll(rot[0], rot[1], rot[2]);
	mCanvas->setRot(rot);

	if (mInputState.checkGamepadKeyChange(1, GHKeyDef::KEY_GP_X, true))
	{
		GHDebugMessage::outputString("pos=\"%f %f %f\" rot=\"%f %f %f\"\n", pos[0], pos[1], pos[2], rot[0], rot[1], rot[2]);
	}
}
