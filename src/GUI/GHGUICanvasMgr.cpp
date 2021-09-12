#include "GHGUICanvasMgr.h"
#include "GHGUICanvas.h"
#include "GHGUIProperties.h"
#include "GHMath/GHTransform.h"
#include "GHMath/GHIntersect.h"
#include "GHInputState.h"
#include "GHScreenPosUnprojector.h"

GHGUICanvasMgr::GHGUICanvasMgr(const GHViewInfo& viewInfo, GHInputState& inputState)
	: mViewInfo(viewInfo)
	, mInputState(inputState)
{
}

GHGUICanvasMgr::~GHGUICanvasMgr(void)
{
	for (auto* predicate : mPointerAllowedPredicates)
	{
		delete predicate;
	}
}

void GHGUICanvasMgr::update(const std::vector<GHGUIWidget::WidgetGroup>& widgetGroups)
{
	// first collect all the active canvases.
	std::set<GHGUICanvas*> canvases;
	for (auto groupIter = widgetGroups.begin(); groupIter != widgetGroups.end(); ++groupIter)
	{
		if ((*groupIter).mIsActive)
		{
			for (size_t i = 0; i < (*groupIter).mWidgets.size(); ++i)
			{
				collect3dCanvases(canvases, (*groupIter).mWidgets[i]->get());
			}
		}
	}

	// then collide the canvases with any pointers.
	mPointerCollisionPoints.clear();
	std::map<unsigned int, float> intersectDepths;
	for (auto canvIter = canvases.begin(); canvIter != canvases.end(); ++canvIter)
	{
		GHGUICanvas* canvas = *canvIter;
		if (!canvas->isInteractable()) continue; // should this check be in collect instead?

		collideCanvas(canvas, mPointerCollisionPoints, intersectDepths);
	}
}

void GHGUICanvasMgr::collect3dCanvases(std::set<GHGUICanvas*>& canvases, const GHGUIWidget* top) const
{
	const GHProperty& canvasProp = top->getPropertyContainer().getProperty(GHGUIProperties::GP_GUICANVAS);
	if (canvasProp.isValid())
	{
		GHGUICanvas* canvas = canvasProp;
		if (canvas && !canvas->is2d())
		{
			canvases.insert(canvas);
		}
	}
	for (size_t i = 0; i < top->getNumChildren(); ++i)
	{
		collect3dCanvases(canvases, top->getChildAtIndex(i)->get());
	}
}

void GHGUICanvasMgr::collideCanvas(GHGUICanvas* canvas, std::map<unsigned int, GHPoint3>& intersects, std::map<unsigned int, float>& intersectDepths) const
{
	assert(canvas);
	GHTransform guiToWorld;
	canvas->clearPointerOverrides();
	canvas->createGuiToWorld(mViewInfo, guiToWorld);

	GHPoint3 ul(0, 0, 0);
	GHPoint3 ur(1, 0, 0);
	GHPoint3 bl(0, 1, 0);
	GHPoint3 br(1, 1, 0);
	guiToWorld.mult(ul, ul);
	guiToWorld.mult(ur, ur);
	guiToWorld.mult(bl, bl);
	guiToWorld.mult(br, br);

	GHTransform worldToGUI = guiToWorld;
	worldToGUI.invert();

	// todo: handle overlapping canvases.

	unsigned int numPoseCollisions = 0;
	for (unsigned int gamepadIdx = 0; gamepadIdx < mInputState.getNumGamepads(); ++gamepadIdx)
	{
		const GHInputStructs::Gamepad& gamepad = mInputState.getGamepad(gamepadIdx);
		if (!gamepad.mActive) {
			continue;
		}
		for (unsigned int poseIdx = 0; poseIdx < gamepad.mPoseables.size(); ++poseIdx)
		{
			const GHInputStructs::Poseable& pose = gamepad.mPoseables[poseIdx];
			if (!pose.mActive || !poseableIsAllowed((int)gamepadIdx, (int)poseIdx, pose))
			{
				continue;
			}
			
			GHPoint3 poseOrigin;
			pose.mPosition.getTranslate(poseOrigin);
			GHPoint3 poseDir(0, 0, 1);
			pose.mPosition.multDir(poseDir, poseDir);

			float intersectDist = 0;
			GHPoint3 intersectNormal;
			if (GHIntersect::collideQuad(poseOrigin, poseDir, ul, ur, bl, br, &intersectDist, &intersectNormal))
			{
				unsigned int magicIdx = mInputState.calcPointerIndexForGamepadPoseable(gamepadIdx, poseIdx);

				const auto prevFind = intersectDepths.find(magicIdx);
				if (prevFind != intersectDepths.end())
				{
					if (prevFind->second < intersectDist)
					{
						continue;
					}
				}
				intersectDepths[magicIdx] = intersectDist;

				// calculate the 3d collision point.
				poseDir *= intersectDist;
				poseOrigin += poseDir;
				intersects[magicIdx] = poseOrigin;

				// calculate the 2d collision point.
				GHPoint3 colGUISpace;
				worldToGUI.mult(poseOrigin, colGUISpace);
				GHPoint2 col2d(colGUISpace[0], colGUISpace[1]);

				canvas->addPointerOverride(col2d, magicIdx);

				numPoseCollisions++;
			}
		}
	}

	// collide cursors.
	// todo: remove duplication with the poseables block.
	GHScreenPosUnprojector posUnprojector(mViewInfo);
	for (unsigned int pointerIdx = 0; pointerIdx < mInputState.getNumPointers(); ++pointerIdx)
	{
		if (!mInputState.getPointerActive(pointerIdx)) {
			continue;
		}
		const GHPoint2 pointerPos2d = mInputState.getPointerPosition(pointerIdx);
		GHPoint3 pointerOrigin;
		GHPoint3 pointerDir;
		posUnprojector.unproject(pointerPos2d, pointerOrigin, pointerDir);

		float intersectDist = 0;
		GHPoint3 intersectNormal;
		if (GHIntersect::collideQuad(pointerOrigin, pointerDir, ul, ur, bl, br, &intersectDist, &intersectNormal))
		{
			// calculate the 3d collision point.
			pointerDir *= intersectDist;
			pointerOrigin += pointerDir;
			intersects[pointerIdx] = pointerOrigin;

			// calculate the 2d collision point.
			GHPoint3 colGUISpace;
			worldToGUI.mult(pointerOrigin, colGUISpace);
			GHPoint2 col2d(colGUISpace[0], colGUISpace[1]);

			canvas->addPointerOverride(col2d, pointerIdx);
		}

	}
}

bool GHGUICanvasMgr::getPointerCollision(unsigned int pointerIdx, GHPoint3& colPos) const
{
	return getPointerCollisionInternal(pointerIdx, colPos);
}

bool GHGUICanvasMgr::getGamepadPoseableCollision(unsigned int gamepadIdx, unsigned int poseableIdx, GHPoint3& colPos) const
{
	unsigned int magicIdx = mInputState.calcPointerIndexForGamepadPoseable(gamepadIdx, poseableIdx);
	return getPointerCollisionInternal(magicIdx, colPos);
}

bool GHGUICanvasMgr::getPointerCollisionInternal(unsigned int modPointerIdx, GHPoint3& colPos) const
{
	auto findIter = mPointerCollisionPoints.find(modPointerIdx);
	if (findIter == mPointerCollisionPoints.end())
	{
		return false;
	}
	colPos = findIter->second;
	return true;
}

void GHGUICanvasMgr::addPointerAllowedPredicate(PointerAllowedPredicate* predicate)
{
	mPointerAllowedPredicates.push_back(predicate);
}

bool GHGUICanvasMgr::poseableIsAllowed(int gamepadIdx, int poseIdx, const GHInputStructs::Poseable& pose) const
{
	for (auto* predicate : mPointerAllowedPredicates)
	{
		if (predicate && !predicate->allowPoseableGUIInteraction((int)gamepadIdx, (int)poseIdx, pose))
		{
			return false;
		}
	}
	return true;
}

