#pragma once
#include "GHXMLObjLoader.h"

class GHInputState;
class GHTimeVal;
class GHTransform;

//<canvasInputMover/>
class GHGUICanvasInputMoverXMLLoader : public GHXMLObjLoader
{
public:
	GHGUICanvasInputMoverXMLLoader(const GHInputState& inputState, const GHTimeVal& timeVal, const GHTransform& frameOfReference);

	virtual void* create(const GHXMLNode& node, GHPropertyContainer& extraData) const;
	GH_NO_POPULATE

private:
	const GHInputState& mInputState;
	const GHTimeVal& mTimeVal;
	const GHTransform& mFrameOfReference;
};

