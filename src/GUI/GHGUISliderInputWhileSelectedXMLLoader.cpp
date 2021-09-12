// Copyright Golden Hammer Software
#include "GHGUISliderInputWhileSelectedXMLLoader.h"
#include "GHGUISliderInputWhileSelected.h"
#include "GHXMLNode.h"
#include "GHPlatform/GHDebugMessage.h"

GHGUISliderInputWhileSelectedXMLLoader::GHGUISliderInputWhileSelectedXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashtable, const GHInputState& inputState, const GHTimeVal& timeVal)
	: mProps(props)
	, mIdFactory(hashtable)
	, mInputState(inputState)
	, mTimeVal(timeVal)
{

}

void* GHGUISliderInputWhileSelectedXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier prop;
	if (!node.readAttrIdentifier("prop", prop, mIdFactory))
	{
		GHDebugMessage::outputString("error loading a GHGUISliderInputWhileSelected: no prop specified");
		return 0;
	}
	
	GHPoint2 range(0, 1);
	node.readAttrFloatArr("range", range.getArr(), 2);

	float speed = 1;
	node.readAttrFloat("speed", speed);

	int dimension = 0;
	node.readAttrInt("dimension", dimension);

	return new GHGUISliderInputWhileSelected(mProps, mInputState, mTimeVal, prop, range, speed, dimension);
}
