// Copyright Golden Hammer Software
#include "GHPropertyToggleTransitionXMLLoader.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLNode.h"
#include "GHString/GHStringIdFactory.h"
#include "GHUtils/GHPropertyToggleTransition.h"
#include "GHXMLObjFactory.h"
#include "GHUtils/GHPropertyContainer.h"

GHPropertyToggleTransitionXMLLoader::GHPropertyToggleTransitionXMLLoader(GHXMLObjFactory& objFactory, const GHStringIdFactory& hashTable,
                                                                         GHPropertyContainer& props)
: mObjFactory(objFactory)
, mIdFactory(hashTable)
, mProps(props)
{
}

void* GHPropertyToggleTransitionXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    const char* propStr = node.getAttribute("prop");
    if (!propStr) {
        GHDebugMessage::outputString("PToggleTrans does not specify a property");
        return 0;
    }
    GHIdentifier propId = mIdFactory.generateHash(propStr);
	GHPropertyContainer* props = &mProps;
	GHIdentifier propMapID;
	if (node.readAttrIdentifier("propertyMap", propMapID, mIdFactory))
	{
		GHPropertyContainer* extraDataPropMap = extraData.getProperty(propMapID);
		if (extraDataPropMap)
		{
			props = extraDataPropMap;
		}
	}

    GHPropertyToggleTransition* ret = new GHPropertyToggleTransition(*props, propId);
    populate(ret, node, extraData);
    return ret;
}

void GHPropertyToggleTransitionXMLLoader::populate(void* obj, const GHXMLNode& node, GHPropertyContainer& extraData) const
{
    GHPropertyToggleTransition* ret = (GHPropertyToggleTransition*)obj;
    
    for (size_t i = 0; i < node.getChildren().size(); ++i)
    {
        GHProperty* currProp = (GHProperty*)mObjFactory.load(*node.getChildren()[i], &extraData);
        if (currProp) {
            ret->addValue(*currProp);
            delete currProp;
        }
        else
        {
            GHDebugMessage::outputString("Failed to find property toggle for %s", node.getName().getChars());
        }
    }
}
