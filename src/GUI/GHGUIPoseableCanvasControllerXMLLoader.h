#pragma once
#include "GHXMLObjLoader.h"

class GHInputState;

//<poseableCanvasController gamepadIndex="1" poseableIndex="0"/>
class GHGUIPoseableCanvasControllerXMLLoader : public GHXMLObjLoader
{
public:
	GHGUIPoseableCanvasControllerXMLLoader(const GHInputState& inputState);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHInputState& mInputState;
};

