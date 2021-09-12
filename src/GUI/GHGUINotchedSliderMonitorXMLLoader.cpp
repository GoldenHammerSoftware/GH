// Copyright Golden Hammer Software
#include "GHGUINotchedSliderMonitorXMLLoader.h"
#include "GHGUINotchedSliderMonitor.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHXMLObjFactory.h"

GHGUINotchedSliderMonitorXMLLoader::GHGUINotchedSliderMonitorXMLLoader(GHPropertyContainer& appProps, const GHXMLObjFactory& xmlObjFactory, const GHStringIdFactory& hashtable)
	: mAppProps(appProps)
	, mXMLObjFactory(xmlObjFactory)
	, mIdFactory(hashtable)
{

}


//<guiNotchedSliderMonitor prop = "GP_DIFFICULTYSLIDERVAL">
//	<notches>
//		<notch val = "0">
//			<transitions>
//				<transitionsGoHere/>
//			</transitions>
//		</notch>
//	</notches>
//</guiNotchedSliderMonitor/>

void* GHGUINotchedSliderMonitorXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier propId;
	if (!node.readAttrIdentifier("prop", propId, mIdFactory))
	{
		GHDebugMessage::outputString("Failed to load a guiNotchedSliderMonitor because there is no \"prop\" identifier to monitor");
		return 0;
	}

	GHGUINotchedSliderMonitor* ret = 0;
	const GHXMLNode* notchesNode = node.getChild("notches", false);
	if (notchesNode)
	{
		ret = new GHGUINotchedSliderMonitor(mAppProps, propId);
		bool didLoadAnyNotches = false;
		const std::vector<GHXMLNode*>& notchesList = notchesNode->getChildren();
		size_t numNotches = notchesList.size();
		for (size_t i = 0; i < numNotches; ++i)
		{
			const GHXMLNode& notchNode = *notchesList[i];
			float val = 0;
			if (notchNode.readAttrFloat("val", val))
			{
				didLoadAnyNotches = true;
				GHGUINotchedSliderMonitor::Notch& notch = ret->createNotch(val);

				const GHXMLNode* transitionListNode = notchNode.getChild("transitions", false);
				if (transitionListNode)
				{
					const std::vector<GHXMLNode*>& transitionList = transitionListNode->getChildren();
					size_t numTransitions = transitionList.size();
					notch.mTransitions.reserve(numTransitions);
					for (size_t j = 0; j < numTransitions; ++j)
					{
						const GHXMLNode& transitionNode = *transitionList[j];
						GHTransition* transition = (GHTransition*)mXMLObjFactory.load(transitionNode, &extraData);
						if (transition) 
						{
							notch.mTransitions.push_back(transition);
						}
						else
						{
							GHDebugMessage::outputString("Failed to load a transition in a guiNotchedSliderMonitor. XML Node Name: %s", transitionNode.getName().getChars());
						}
					}
				}
			}
			else
			{
				GHDebugMessage::outputString("Failed to load a notch in a guiNotchedSliderMonitor because it did not have a \"val\" attribute");
			}
		}

		if (!didLoadAnyNotches)
		{
			delete ret;
			ret = 0;
			GHDebugMessage::outputString("Failed to load a guiNotchedSliderMonitor because no notches were able to be loaded");
		}
	}
	else
	{
		GHDebugMessage::outputString("Failed to load a guiNotchedSliderMonitor because there is no \"notches\" xml node");
	}
	return ret;
}
