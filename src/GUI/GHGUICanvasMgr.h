#pragma once

#include "GHGUIWidget.h"
#include <vector>
#include <map>
#include <set>
#include "GHMath/GHPoint.h"
#include "Base/GHInputState.h"

class GHGUICanvas;
class GHViewInfo;

class GHGUICanvasMgr
{
public:
	GHGUICanvasMgr(const GHViewInfo& viewInfo, GHInputState& inputState);
	~GHGUICanvasMgr(void);

	void update(const std::vector<GHGUIWidget::WidgetGroup>& widgetGroups);

	bool getPointerCollision(unsigned int pointerIdx, GHPoint3& colPos) const;
	bool getGamepadPoseableCollision(unsigned int gamepadIdx, unsigned int poseableIdx, GHPoint3& colPos) const;

	class PointerAllowedPredicate
	{
	public:
		virtual ~PointerAllowedPredicate(void) {}
		virtual bool allowPoseableGUIInteraction(int gamepadIndex, int poseableIndex, const GHInputStructs::Poseable& poseable) = 0;
	};

	void addPointerAllowedPredicate(PointerAllowedPredicate* predicate);

private:
	// recurse and collect all the 3d canvases in a widget tree.
	void collect3dCanvases(std::set<GHGUICanvas*>& canvases, const GHGUIWidget* top) const;
	void collideCanvas(GHGUICanvas* canvas, std::map<unsigned int, GHPoint3>& intersects, std::map<unsigned int, float>& intersectDepths) const;
	
	bool getPointerCollisionInternal(unsigned int modPointerIdx, GHPoint3& colPos) const;

	bool poseableIsAllowed(int gamepadIndex, int poseableIndex, const GHInputStructs::Poseable& poseable) const;

private:
	const GHViewInfo& mViewInfo;
	GHInputState& mInputState;

	// a map of magic pointer/poseable index to collision point.
	// collected during update, valid for 1 frame.
	std::map<unsigned int, GHPoint3> mPointerCollisionPoints;
	std::vector<PointerAllowedPredicate*> mPointerAllowedPredicates;
};
