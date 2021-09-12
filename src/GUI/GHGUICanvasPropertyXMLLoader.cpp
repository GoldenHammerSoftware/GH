// Copyright Golden Hammer Software
#include "GHGUICanvasPropertyXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"
#include "GHGUICanvas.h"
#include "GHXMLObjLoaderGHM.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHBaseIdentifiers.h"
#include "GHScreenInfo.h"

/*
<guiCanvas is2d=true pos="0 0 0" rot="0 0 0" scale="1 1 1", billboardType=BT_NONE/>
*/
GHGUICanvasPropertyXMLLoader::GHGUICanvasPropertyXMLLoader(const GHScreenInfo& mainScreenInfo)
	: mMainScreenInfo(mainScreenInfo)
{
}

void* GHGUICanvasPropertyXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	bool is2d = true;
	bool isInteractable = true;
	GHPoint3 pos;
	GHPoint3 rot;
	GHPoint3 scale(1.0f, 1.0f, 1.0f);

	node.readAttrBool("is2d", is2d);
	node.readAttrBool("isInteractable", isInteractable);
	node.readAttrFloatArr("pos", pos.getArr(), 3);
	node.readAttrFloatArr("rot", rot.getArr(), 3);
	node.readAttrFloatArr("scale", scale.getArr(), 3);

	const char* billboardStr = node.getAttribute("billboardType");
	GHMDesc::BillboardType bt;
	GHXMLObjLoaderGHM::loadBillboardType(billboardStr, bt);

	GHScreenInfo* screenInfo = createScreenInfo(is2d, scale);
	
	GHGUICanvas* canvas = new GHGUICanvas(is2d, isInteractable, pos, rot, scale, bt, screenInfo);

	if ((int)extraData.getProperty(GHBaseIdentifiers::P_NOLOADASPROPERTY) != 0)
	{
		return canvas;
	}
	return new GHProperty(canvas, new GHRefCountedType<GHGUICanvas>(canvas));
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// Explanations of magic numbers:
// 1) Screen Size Multipliers
//This is the canvas size of guibg.xml, which I think is more or less supposed to mirror the default window size:
//<guiCanvas is2d = "false" pos = "-1 1.25 2.6" rot = "0 0 0" scale = "3.6 2.1 1" billboardType = "BT_NONE" / >
//This is the ScreenInfo for the default window size
// mScreenInfo.mSize = 1457, 789
//	  mPixelSizeMult = 0.00100000005, 0
//		  mPixelSize = 1.21384621
//	   mMaxPixelSize = 1.21384621
// mMinimumPixelHeight=650
// mAspectRatio		= 1.84664130
// Therefore, I think the default screen info (assuming a canvas scale of 1 1 (z scale seems irrelevant)) should be:
// mScreenInfo.mSize = 404.722222222, 375.714285714. 
// 2) Pixel Size Factor was derived experimentally based on what made the scoll panel work well for the VR TSB2 puzzle mode select HUD element.
GHScreenInfo* GHGUICanvasPropertyXMLLoader::createScreenInfo(bool is2d, const GHPoint3& scale) const
{
	//canvases only need a screen info if they are 3D (maybe just pass a non-owned pointer to the global screen info if 2D?)
	if (is2d) {
		return nullptr;
	}

	//Copying the main screen info to keep stuff like the pixel size mult
	GHScreenInfo* screenInfo = new GHScreenInfo;

	//Explicitly setting the size here based on a one-time window size because resizing 
	// the flat screen should *not* change the intended size of the canvas in 3D. 
	screenInfo->setSize(GHPoint2i((int)(404.722222222*scale[0]+0.5f), (int)(375.714285714*scale[1]+0.5f)));
	screenInfo->setPixelSizeFactor(0.00091358024f, 0);
	return screenInfo;
}