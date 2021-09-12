#include "GHGUIWidgetRendererModelXMLLoader.h"
#include "GHXMLObjFactory.h"
#include "GHGUIWidgetRendererModel.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUIWidgetRendererModelXMLLoader::GHGUIWidgetRendererModelXMLLoader(const GHXMLObjFactory& objFactory)
	: mObjFactory(objFactory)
{

}

void* GHGUIWidgetRendererModelXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	if (!node.getChildren().size())
	{
		GHDebugMessage::outputString("Error in GHGUIWidgetRendererModelXMLLoader: the first child of this xml node must be the model");
		return 0;
	}

	GHModel* model = (GHModel*)mObjFactory.load(*node.getChildren()[0], &extraData);

	if (!model)
	{
		GHDebugMessage::outputString("Error in GHGUIWidgetRendererModelXMLLoader: failed to load model");
		return 0;
	}

	GHPoint3 pos(0, 0, 0);
	node.readAttrFloatArr("pos", pos.getArr(), 3);

	GHPoint3 scale(1, 1, 1);
	node.readAttrFloatArr("scale", scale.getArr(), 3);

	return new GHGUIWidgetRendererModel(model, pos, scale);
}
