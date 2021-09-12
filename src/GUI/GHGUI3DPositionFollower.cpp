// Copyright Golden Hammer Software
#include "GHGUI3DPositionFollower.h"
#include "GHViewInfo.h"
#include "GHGUIPanel.h"
#include "GHCamera.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"

GHGUI3DPositionFollower::GHGUI3DPositionFollower(const GHPropertyContainer& props,
												const GHIdentifier& pointIndex,
												const GHViewInfo& viewInfo,
												GHGUIPanel& targetPanel)
: mProps(props)
, mPointIndex(pointIndex)
, mUnprojector(viewInfo)
, mTargetPanel(targetPanel)
{

}

void GHGUI3DPositionFollower::onActivate(void)
{
	update();
}

void GHGUI3DPositionFollower::onDeactivate(void)
{

}

void GHGUI3DPositionFollower::update(void)
{
	GHPoint3* worldPos = mProps.getProperty(mPointIndex);
	if (!worldPos) {
		return;
	}

	GHGUIPosDesc posDesc = mTargetPanel.getPosDesc();

	posDesc.mSizeAlign[0] = posDesc.mSizeAlign[1] = GHAlign::A_CENTER;
	posDesc.mOffset[0] = posDesc.mOffset[1] = 0;

	GHPoint2 screenPos;
	mUnprojector.project(*worldPos, posDesc.mAlign);

	mTargetPanel.setPosDesc(posDesc);
	mTargetPanel.updatePosition();
}
