// Copyright Golden Hammer Software
#include "GHGUICanvasSwitchXMLLoader.h"
#include "GHXMLNode.h"
#include "GHUtils/GHProperty.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUICanvas.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"
#include "GHGUICanvasSwitch.h"
#include "GHBaseIdentifiers.h"

GHGUICanvasSwitchXMLLoader::GHGUICanvasSwitchXMLLoader(const GHPropertyContainer& props, GHXMLObjFactory& objFactory, const GHStringIdFactory& idFactory)
	: mProps(props)
	, mObjFactory(objFactory)
	, mIdFactory(idFactory)
{
}

/*
<guiCanvasSwitch propId="GP_HMDACTIVE">
	<guiCanvas.../>
	<guiCanvas.../>
</guiCanvasSwitch>
*/
void* GHGUICanvasSwitchXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (node.getChildren().size() != 2)
	{
		GHDebugMessage::outputString("Wrong number of children for GHGUICanvasSwitchXMLLoader");
		return 0;
	}
	GHIdentifier propId = 0;
	if (!node.readAttrIdentifier("propId", propId, mIdFactory))
	{
		GHDebugMessage::outputString("No propId specified for canvas switch");
		return 0;
	}
	// the canvas loader loads properties...
	GHPropertyContainer canvasLoadProps = extraData;
	canvasLoadProps.setProperty(GHBaseIdentifiers::P_NOLOADASPROPERTY, 1);
	IGHGUICanvas* onCanvas = (IGHGUICanvas*)mObjFactory.load(*node.getChildren()[0], &canvasLoadProps);
	IGHGUICanvas* offCanvas = (IGHGUICanvas*)mObjFactory.load(*node.getChildren()[1], &canvasLoadProps);
	if (!onCanvas || !onCanvas)
	{
		GHDebugMessage::outputString("Failed to load on/off canvases for switch");
		if (onCanvas) delete onCanvas;
		if (offCanvas) delete offCanvas;
		return 0;
	}
	GHGUICanvasSwitch* ret = new GHGUICanvasSwitch(mProps, propId, onCanvas, offCanvas);
	if ((int)extraData.getProperty(GHBaseIdentifiers::P_NOLOADASPROPERTY) != 0)
	{
		return ret;
	}
	return new GHProperty(ret, new GHRefCountedType<GHGUICanvasSwitch>(ret));
}
