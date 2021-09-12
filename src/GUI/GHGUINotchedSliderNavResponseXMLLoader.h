// Copyright Golden Hammer Software
#pragma once
#include "GHXMLObjLoader.h"

class GHStringIdFactory;

//<guiNotchedSliderResponse prop="GP_DIFFICULTYSLIDERVAL" vals = "0 .33 .66 1" / >
class GHGUINotchedSliderNavResponseXMLLoader : public GHXMLObjLoader
{
public:
	GHGUINotchedSliderNavResponseXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashtable);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	GHPropertyContainer& mProps;
	const GHStringIdFactory& mIdFactory;
};
