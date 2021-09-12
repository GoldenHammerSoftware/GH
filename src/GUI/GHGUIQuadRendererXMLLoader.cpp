// Copyright Golden Hammer Software
#include "GHGUIQuadRendererXMLLoader.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHGUIQuadRenderer.h"
#include "GHMath/GHPi.h"

GHGUIQuadRendererXMLLoader::GHGUIQuadRendererXMLLoader(const GHVBFactory& vbFactory,
	const GHXMLObjFactory& objFactory, const GHScreenInfo& screenInfo)
	: mVBFactory(vbFactory)
	, mObjFactory(objFactory)
	, mScreenInfo(screenInfo)
{
}

void* GHGUIQuadRendererXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (!node.getChildren().size()) return 0;
	GHMaterial* mat = (GHMaterial*)mObjFactory.load(*node.getChildren()[0]);
	if (!mat) return 0;

	GHPoint2 minUV(0, 0);
	GHPoint2 maxUV(1, 1);
	node.readAttrFloatArr("minUV", minUV.getArr(), 2);
	node.readAttrFloatArr("maxUV", maxUV.getArr(), 2);

	float drawOrderOffset = 0;
	node.readAttrFloat("drawOrderOffset", drawOrderOffset);

	float drawLayer = 0;
	node.readAttrFloat("drawLayer", drawLayer);

	GHPoint2i gridSize(1, 1);
	node.readAttrIntArr("gridSize", gridSize.getArr(), 2);

	float uvAngle = 0;
	if (node.readAttrFloat("uvAngle", uvAngle))
	{
		uvAngle *= GHPI;
	}

    GHGUIQuadRenderer* ret = new GHGUIQuadRenderer(mScreenInfo, mVBFactory, mat, minUV, maxUV, drawOrderOffset, drawLayer, gridSize, uvAngle);
    
    return ret;
}

