// Copyright Golden Hammer Software
#include "GHGeometryRenderable.h"
#include "GHRenderPass.h"
#include "GHGeometry.h"
#include "GHPlatform/GHRunnable.h"
#include "GHPlatform/GHStlUtil.h"
#include "GHMath/GHSphere.h"
#include "GHSphereBounds.h"
#include <algorithm>
#include "GHCamera.h"
#include "GHMaterial.h"

GHGeometryRenderable::GHGeometryRenderable(void)
: mRenderCallback(0)
{
}

GHGeometryRenderable::~GHGeometryRenderable(void)
{
    GHStlUtil::deletePointerList(mGeometry);
}

void GHGeometryRenderable::collect(GHRenderPass& pass, const GHFrustum* frustum)
{
    if (!mGeometry.size()) {
        return;
    }
    bool isVisible = true;
    if (mBounds.size() && frustum)
    {
        // todo: make a bounds->cull function
        isVisible = false;
        for (size_t i = 0; i < mBounds.size(); ++i) {
            const GHSphere& sphere = mBounds[i]->getSphere();
            GHVisType::VisType visType = frustum->checkVisible(sphere.mCenter, sphere.mRadius);
            if (visType != GHVisType::VT_CULL) {
                isVisible = true;
                break;
            }
        }
    }

    if (isVisible) {
        if (mRenderCallback) {
            mRenderCallback->run();
        }
        
        GHRenderList& list = pass.getRenderList();
        list.setEntityData(mEntityData);

		const GHCamera& cam = pass.getCamera();
		GHPoint3 camPos;
		cam.getTransform().getTranslate(camPos);

        for (size_t i = 0; i < mGeometry.size(); ++i)
        {
			float dist = 0.0f;
            if (mGeometry[i]->getBounds())
            {
                const GHSphere& sphere = mGeometry[i]->getBounds()->getSphere();
                GHVisType::VisType visType = pass.getViewInfo(GHViewInfo::EYE_MONOCLE).getFrustum().checkVisible(sphere.mCenter, sphere.mRadius);
                if (visType == GHVisType::VT_CULL) {
                    continue;
                }

				// todo? don't calc dist unless there's an LOD.
				if (mGeometry[i]->getMaterial() && mGeometry[i]->getMaterial()->hasLOD())
				{
					GHPoint3 posDiff(sphere.mCenter);
					posDiff -= camPos;
					// todo: use lenSqr
					dist = posDiff.length();
					dist -= sphere.mRadius;
					if (dist < 0) dist = 0;
				}
            }
            mGeometry[i]->addToRenderList(list, mDrawOrder, dist);
        }
    }
}

void GHGeometryRenderable::addGeometry(GHGeometry* geo)
{
    mGeometry.push_back(geo);
}

void GHGeometryRenderable::removeGeometry(GHGeometry* geo)
{
    std::vector<GHGeometry*>::iterator iter = std::find(mGeometry.begin(), mGeometry.end(), geo);
    if (iter != mGeometry.end())
    {
        mGeometry.erase(iter);
    }
}



