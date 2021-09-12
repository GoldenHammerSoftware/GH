// Copyright Golden Hammer Software
#pragma once

#include "GHGUIWidgetRenderer.h"
#include "GHMath/GHPoint.h"

class GHMaterial;
class GHVBFactory;
class GHVertexBuffer;
class GHScreenInfo;

// a widget renderer for drawing a material on a 2d screen quad.
class GHGUIQuadRenderer : public GHGUIWidgetRenderer
{
public:
    GHGUIQuadRenderer(const GHScreenInfo& screenInfo,
                      const GHVBFactory& vbFactory, GHMaterial* material,
                      const GHPoint2& minUV, const GHPoint2& maxUV,
                      float drawOrderOffset, float drawLayer,
					  const GHPoint2i& gridSize, float uvAngleInRadians);
    ~GHGUIQuadRenderer(void);

    virtual void collect(GHRenderPass& pass, const GHFrustum* frustum);
    virtual void updateRect(const GHRect<float,2>& rect, float angleRadians);
    
private:
    const GHScreenInfo& mScreenInfo;
    GHMaterial* mMaterial;
    GHVertexBuffer* mVB;

	GHRect<float, 2> mUV;
	float mUVAngleInRadians;

	// number of quads in the grid.
	GHPoint2i mGridSize;
};
