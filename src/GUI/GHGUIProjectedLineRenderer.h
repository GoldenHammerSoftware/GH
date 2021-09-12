// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidgetRenderer.h"
#include "GHMath/GHPoint.h"
#include <vector>
#include "GHScreenPosUnprojector.h"

class GHVBFactory;
class GHMaterial;
class GHVertexBuffer;

//A widget renderer that draws connected line segments denoted by 
// a series of 3D points. The final render happens in the 2D positions
// projected from the points using the current view transforms.
//Note that any points added via addPoint will not be reflected in the render unless
// generateGeometry is called. Those two functions exist mainly for loading.
class GHGUIProjectedLineRenderer : public GHGUIWidgetRenderer
{
public:
	GHGUIProjectedLineRenderer(GHMaterial* material,
							   const GHViewInfo& viewInfo,
							   float thickness,
							   float drawOrderOffset, float drawLayer);
	virtual ~GHGUIProjectedLineRenderer(void);

	void addPoint(const GHPoint3& point);
	void generateGeometry(const GHVBFactory& vbFactory);

	virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
	virtual void updateRect(const GHRect<float, 2>& rect);

private:
	void updatePositions(void);

private:
	GHMaterial* mMaterial;
	GHVertexBuffer* mVB;
	GHScreenPosUnprojector mUnprojector;

	std::vector<GHPoint3> mPoints;

	float mThickness;
};
