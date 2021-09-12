// Copyright Golden Hammer Software
#pragma once
#include "GHXMLObjLoader.h"

class GHStringIdFactory;
class GHInputState;
class GHTimeVal;

//<guiSliderInput						These values should match the values for the slider class (yes, at the moment this means the information is duplicated)
//	prop = "GP_VOLUME"				<-- the GHIdentifier into the app's property map which is modified by the slider
//  range = "0 1"					<-- the range of values for the property (usually 0-1)
//  speed = ".1"					<-- a speed multiplier affecting how fast input may move the slider
//  dimension="0" / >				<-- the dimension (0 for horizontal, 1 for vertical) to check the input for. In general this should match the dimension along which the slider moves.
class GHGUISliderInputWhileSelectedXMLLoader : public GHXMLObjLoader
{
public:

	GHGUISliderInputWhileSelectedXMLLoader(GHPropertyContainer& props, const GHStringIdFactory& hashtable, const GHInputState& inputState, const GHTimeVal& timeVal);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE


private:
	GHPropertyContainer& mProps;
	const GHStringIdFactory& mIdFactory;
	const GHInputState& mInputState;
	const GHTimeVal& mTimeVal;
};
