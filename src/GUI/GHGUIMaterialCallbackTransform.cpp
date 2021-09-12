// Copyright Golden Hammer Software
#include "GHGUIMaterialCallbackTransform.h"
#include "GHMaterial.h"
#include "GHViewInfo.h"
#include "GHMaterialParamHandle.h"
#include "GHGUICanvas.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"

GHGUIMaterialCallbackTransform::GHGUIMaterialCallbackTransform(GHMaterialParamHandle* guiInWorldHandle)
	: mGUIInWorldHandle(guiInWorldHandle)
{
}

GHGUIMaterialCallbackTransform::~GHGUIMaterialCallbackTransform(void)
{
	delete mGUIInWorldHandle;
}

void GHGUIMaterialCallbackTransform::apply(const void* data)
{
	GHMaterialCallback::CBVal_PerObj* cbArg = (GHMaterialCallback::CBVal_PerObj*)data;

	if (mGUIInWorldHandle)
	{
		GHGUICanvas* canvas = 0;
		if (cbArg && cbArg->mProps)
		{
			const GHProperty& canvasProp = cbArg->mProps->getProperty(GHGUIProperties::GP_GUICANVAS);
			if (canvasProp.isValid())
			{
				canvas = canvasProp;
			}
		}

		if (!canvas || canvas->is2d())
		{
			// draw the 2d projected gui.
			mGUIInWorldHandle->setValue(GHMaterialParamHandle::HT_MAT16, cbArg->mViewInfo.getDeviceTransforms().mPlatformGUITrans.getMatrix().getCoords());
		}
		else
		{
			// move the gui into the world at a fixed location and size.
			GHTransform guiInWorldProj;
			canvas->createGuiToWorld(cbArg->mViewInfo, guiInWorldProj);
			guiInWorldProj.mult(cbArg->mViewInfo.getDeviceTransforms().mViewProjTransform, guiInWorldProj);
			mGUIInWorldHandle->setValue(GHMaterialParamHandle::HT_MAT16, guiInWorldProj.getMatrix().getCoords());
		}
	}
}
