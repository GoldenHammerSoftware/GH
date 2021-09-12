// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"
#include "GHMath/GHPoint.h"

class GHScreenInfo;

/*
<guiCanvas is2d=true pos="0 0 0" rot="0 0 0" scale="1 1 1", billboardType=BT_NONE/>
*/
class GHGUICanvasPropertyXMLLoader : public GHXMLObjLoader
{
public:
	GHGUICanvasPropertyXMLLoader(const GHScreenInfo& mainScreenInfo);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;

private:
	GHScreenInfo* createScreenInfo(bool is2d, const GHPoint3& scale) const;

private:
	const GHScreenInfo& mMainScreenInfo;
};
