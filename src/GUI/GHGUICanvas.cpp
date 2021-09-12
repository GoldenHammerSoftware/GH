#include "GHGUICanvas.h"
#include "GHMath/GHTransform.h"
#include "GHViewInfo.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHScreenInfo.h"

GHGUICanvas::GHGUICanvas(bool is2d, bool isInteractable, const GHPoint3& pos, const GHPoint3& rot, const GHPoint3& scale, GHMDesc::BillboardType bt, GHScreenInfo* screenInfo)
	: IGHGUICanvas()
	, mIs2d(is2d)
	, mInteractable(isInteractable)
	, mPos(pos)
	, mRot(rot)
	, mScale(scale)
	, mBillboardType(bt)
	, mScreenInfo(screenInfo)
{
}

GHGUICanvas::~GHGUICanvas(void)
{
	delete mScreenInfo;
}

void GHGUICanvas::createGuiToWorld(const GHViewInfo& viewInfo, GHTransform& outTrans) const
{
	outTrans.becomeIdentity();

	GHTransform scaleTrans;
	scaleTrans.becomeScaleMatrix(-mScale[0], -mScale[1], mScale[2]);

	GHTransform posTrans;
	posTrans.becomeIdentity();
	posTrans.setTranslate(GHPoint3(mPos[0], mPos[1], mPos[2]));

	GHTransform centerTrans;
	centerTrans.becomeIdentity();
	centerTrans.setTranslate(GHPoint3(-0.5, -0.5, 0.0));

	outTrans.mult(centerTrans, outTrans);
	outTrans.mult(scaleTrans, outTrans);

	if (mBillboardType == GHMDesc::BT_FULL)
	{
        // This code does not seem to work correctly.
		outTrans.mult(viewInfo.getDeviceTransforms().mBillboardTransform, outTrans);
	}
	else if (mBillboardType == GHMDesc::BT_UP)
	{
		outTrans.mult(viewInfo.getDeviceTransforms().mBillboardUpTransform, outTrans);
	}
	else
	{
		GHTransform rotTrans;
		rotTrans.becomeYawPitchRollRotation(mRot[0], mRot[1], mRot[2]);
		outTrans.mult(rotTrans, outTrans);
	}
	
	outTrans.mult(posTrans, outTrans);
}

void GHGUICanvas::clearPointerOverrides(void)
{
	mPointerOverrides.clear();
}

void GHGUICanvas::addPointerOverride(const GHPoint2& pos, unsigned int pointerIndex)
{
	GHInputStructs::Pointer newPointer;
	newPointer.mPointerIndex = pointerIndex;
	newPointer.mPosition = pos;
	newPointer.mActive = true;
	mPointerOverrides.push_back(newPointer);
}

const GHInputStructs::PointerList& GHGUICanvas::getPointerOverrides(void)
{
	return mPointerOverrides;
}

void GHGUICanvas::setPos(const GHPoint3& pos)
{
	mPos = pos;
}

void GHGUICanvas::setRot(const GHPoint3& rot)
{
	mRot = rot;
}

void GHGUICanvas::setScale(const GHPoint3& scale)
{
	mScale = scale;
}

IGHGUICanvas* IGHGUICanvas::get3dCanvas(const GHPropertyContainer& props)
{
	const GHProperty& canvasProp = props.getProperty(GHGUIProperties::GP_GUICANVAS);
	if (canvasProp.isValid())
	{
		IGHGUICanvas* ret = canvasProp;
		if (ret && !ret->is2d())
		{
			return ret;
		}
	}
	return 0;
}
