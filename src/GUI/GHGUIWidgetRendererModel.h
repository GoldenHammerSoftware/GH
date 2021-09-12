#pragma once
#include "GHGUIWidgetRenderer.h"


class GHModel;

class GHGUIWidgetRendererModel : public GHGUIWidgetRenderer 
{
public:
	GHGUIWidgetRendererModel(GHModel* model, const GHPoint3& pos, const GHPoint3& scale);
	virtual ~GHGUIWidgetRendererModel(void);

	virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
	virtual void updateRect(const GHRect<float, 2>& rect, float angleRadians);
	virtual void updateDrawOrder(float baseGUIDrawOrder, float offset);

private:
	GHModel* mModel;
	GHPoint3 mPos;
	GHPoint3 mScale;
};

