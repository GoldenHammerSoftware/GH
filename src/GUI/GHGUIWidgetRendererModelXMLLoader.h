#pragma once

#include "GHXMLObjLoader.h"

class GHXMLObjFactory;

//first child is the model
//<guiWidgetRendererModel>
//   <resource file="canvasbgmodel.xml"/>
//<guiWidgetRendererModel>
class GHGUIWidgetRendererModelXMLLoader : public GHXMLObjLoader
{
public:
	GHGUIWidgetRendererModelXMLLoader(const GHXMLObjFactory& objFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHXMLObjFactory& mObjFactory;
};
