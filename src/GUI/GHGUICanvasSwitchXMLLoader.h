// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHXMLObjFactory;
class GHStringIdFactory;

/*
<guiCanvasSwitch propId="GP_HMDACTIVE">
	<guiCanvas.../>
	<guiCanvas.../>
</guiCanvasSwitch>
*/
class GHGUICanvasSwitchXMLLoader : public GHXMLObjLoader
{
public:
	GHGUICanvasSwitchXMLLoader(const GHPropertyContainer& props, GHXMLObjFactory& objFactory, const GHStringIdFactory& idFactory);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;

private:
	const GHPropertyContainer& mProps;
	GHXMLObjFactory& mObjFactory;
	const GHStringIdFactory& mIdFactory;
};
