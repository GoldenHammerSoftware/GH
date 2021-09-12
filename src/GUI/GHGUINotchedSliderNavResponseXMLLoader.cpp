// Copyright Golden Hammer Software
#include "GHGUINotchedSliderNavResponseXMLLoader.h"
#include "GHGUINotchedSliderNavResponse.h"
#include "GHString/GHIdentifier.h"
#include "GHXMLNode.h"

GHGUINotchedSliderNavResponseXMLLoader::GHGUINotchedSliderNavResponseXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashtable)
	: mProps(props)
	, mIdFactory(hashtable)
{

}

void* GHGUINotchedSliderNavResponseXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHIdentifier propID;
	node.readAttrIdentifier("prop", propID, mIdFactory);

	std::vector<float> vals;
	node.readAttrFloatList("vals", vals);

	return new GHGUINotchedSliderNavResponse(mProps, propID, vals);
}
