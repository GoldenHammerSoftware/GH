#include "GHGUICanvasInputMoverXMLLoader.h"
#include "GHGUICanvasInputMover.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHGUIWidget.h"

GHGUICanvasInputMoverXMLLoader::GHGUICanvasInputMoverXMLLoader(const GHInputState& inputState, const GHTimeVal& timeVal, const GHTransform& frameOfReference)
	: mInputState(inputState)
	, mTimeVal(timeVal)
	, mFrameOfReference(frameOfReference)
{
}

void* GHGUICanvasInputMoverXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	GHGUIWidget* widget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);

	if (!widget)
	{
		GHDebugMessage::outputString("Error creating GHGUICanvasInputMoverXMLLoader: no widget on extraData");
		return 0;
	}

	return new GHGUICanvasInputMover(mInputState, mTimeVal, mFrameOfReference, *widget);
}

