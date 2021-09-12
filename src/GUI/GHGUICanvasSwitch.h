#pragma once

#include "GHGUICanvas.h"

class GHPropertyContainer;

// A set of gui canvases that switches on a prop
class GHGUICanvasSwitch : public IGHGUICanvas
{
public:
	GHGUICanvasSwitch(const GHPropertyContainer& props, const GHIdentifier& propId, IGHGUICanvas* onCanvas, IGHGUICanvas* offCanvas);
	~GHGUICanvasSwitch(void);

	bool is2d(void) const;
	bool isInteractable(void) const;

	void createGuiToWorld(const GHViewInfo& viewInfo, 
						  GHTransform& outTrans) const;

	void clearPointerOverrides(void);
	void addPointerOverride(const GHPoint2& pos, unsigned int pointerIndex);
	const GHInputStructs::PointerList& getPointerOverrides(void);

	void setPos(const GHPoint3& pos);
	void setRot(const GHPoint3& rot);
	void setScale(const GHPoint3& scale);

	const GHPoint3& getPos(void) const;
	const GHPoint3& getRot(void) const;
	const GHPoint3& getScale(void) const;

	const GHScreenInfo* getScreenInfo(void) const;

private:
	IGHGUICanvas* getActiveCanvas(void) const;

private:
	const GHPropertyContainer& mProps;
	GHIdentifier mPropId;
	IGHGUICanvas* mCanvases[2];
};
