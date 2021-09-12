// Copyright Golden Hammer Software
#pragma once

#include "GHXMLObjLoader.h"

class GHPropertyContainer;
class GHViewInfo;
class GHStringIdFactory;

//<gui3DPosFollower prop="GP_TARGETPOSITION"/>
class GHGUI3DPositionFollowerXMLLoader : public GHXMLObjLoader
{
public:
	GHGUI3DPositionFollowerXMLLoader(const GHPropertyContainer& props,
									 const GHViewInfo& viewInfo,
									 const GHStringIdFactory& stringHashtable);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE;

private:
	const GHPropertyContainer& mProps;
	const GHViewInfo& mViewInfo;
	const GHStringIdFactory& mStringHashtable;
};
