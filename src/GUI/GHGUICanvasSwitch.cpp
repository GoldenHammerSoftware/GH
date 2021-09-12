#include "GHGUICanvasSwitch.h"
#include "GHUtils/GHPropertyContainer.h"
#include <assert.h>

GHGUICanvasSwitch::GHGUICanvasSwitch(const GHPropertyContainer& props, const GHIdentifier& propId, 
	IGHGUICanvas* onCanvas, IGHGUICanvas* offCanvas)
	: mProps(props)
	, mPropId(propId)
{
	assert(onCanvas);
	assert(offCanvas);
	mCanvases[0] = onCanvas;
	mCanvases[1] = offCanvas;
}

GHGUICanvasSwitch::~GHGUICanvasSwitch(void)
{
	for (size_t i = 0; i < 2; ++i)
	{
		delete mCanvases[i];
	}
}

bool GHGUICanvasSwitch::is2d(void) const
{
	return getActiveCanvas()->is2d();
}

bool GHGUICanvasSwitch::isInteractable(void) const
{
	return getActiveCanvas()->isInteractable();
}

void GHGUICanvasSwitch::createGuiToWorld(const GHViewInfo& viewInfo, GHTransform& outTrans) const
{
	return getActiveCanvas()->createGuiToWorld(viewInfo, outTrans);
}

void GHGUICanvasSwitch::clearPointerOverrides(void)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mCanvases[i]->clearPointerOverrides();
	}
}

void GHGUICanvasSwitch::addPointerOverride(const GHPoint2& pos, unsigned int pointerIndex)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mCanvases[i]->addPointerOverride(pos, pointerIndex);
	}
}

const GHInputStructs::PointerList& GHGUICanvasSwitch::getPointerOverrides(void)
{
	return getActiveCanvas()->getPointerOverrides();
}

void GHGUICanvasSwitch::setPos(const GHPoint3& pos)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mCanvases[i]->setPos(pos);
	}
}

void GHGUICanvasSwitch::setRot(const GHPoint3& rot)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mCanvases[i]->setRot(rot);
	}
}

void GHGUICanvasSwitch::setScale(const GHPoint3& scale)
{
	for (size_t i = 0; i < 2; ++i)
	{
		mCanvases[i]->setScale(scale);
	}
}

const GHPoint3& GHGUICanvasSwitch::getPos(void) const
{
	return getActiveCanvas()->getPos();
}

const GHPoint3& GHGUICanvasSwitch::getRot(void) const
{
	return getActiveCanvas()->getRot();
}

const GHPoint3& GHGUICanvasSwitch::getScale(void) const
{
	return getActiveCanvas()->getScale();
}

const GHScreenInfo* GHGUICanvasSwitch::getScreenInfo(void) const
{
	return getActiveCanvas()->getScreenInfo();
}

IGHGUICanvas* GHGUICanvasSwitch::getActiveCanvas(void) const
{
	bool propVal = (bool)mProps.getProperty(mPropId);
	if (propVal)
	{
		return mCanvases[0];
	}
	return mCanvases[1];
}
