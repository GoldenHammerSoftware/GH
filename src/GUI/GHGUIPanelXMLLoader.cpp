// Copyright Golden Hammer Software
#include "GHGUIPanelXMLLoader.h"
#include "GHGUIPanel.h"
#include "GHXMLNode.h"
#include "GHXMLObjFactory.h"
#include "GHGUIWidgetLoader.h"
#include "GHUtils/GHPropertyStacker.h"
#include "GHGUIProperties.h"
#include "GHGUIWidgetRenderer.h"

GHGUIPanelXMLLoader::GHGUIPanelXMLLoader(const GHXMLObjFactory& xmlFactory,
                                         GHGUIRenderable& renderer, 
                                         const GHGUIRectMaker& rectMaker,
                                         const GHGUIWidgetLoader& widgetLoader)
: mRenderer(renderer)
, mRectMaker(rectMaker)
, mXMLFactory(xmlFactory)
, mWidgetLoader(widgetLoader)
{
}

void* GHGUIPanelXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIPanel* panel = new GHGUIPanel(mRenderer, mRectMaker);
    GHGUIWidgetResource* ret = new GHGUIWidgetResource(panel);
    populate(ret, node, extraData);
    return ret;
}

void GHGUIPanelXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHGUIWidgetResource* res = (GHGUIWidgetResource*)obj;
    GHGUIPanel* ret = (GHGUIPanel*)res->get();

    GHPropertyStacker parentStack(extraData, GHGUIProperties::GP_PARENTWIDGET, GHProperty(ret));
    GHPropertyStacker parentResStack(extraData, GHGUIProperties::GP_PARENTWIDGETRES, GHProperty(res));
    
    mWidgetLoader.populate(node, *ret, extraData);
    
    const GHXMLNode* renderNode = node.getChild("renderable", false);
    if (renderNode && renderNode->getChildren().size()) 
    {
        GHGUIWidgetRenderer* renderer = (GHGUIWidgetRenderer*)mXMLFactory.load(*(renderNode->getChildren()[0]));
        if (renderer) {
            ret->setWidgetRenderer(renderer);
			renderer->setWidget(ret);
        }
    }
}

