#include "GHGUIPoseableCanvasControllerXMLLoader.h"
#include "GHGUIPoseableCanvasController.h"
#include "GHInputState.h"
#include "GHXMLNode.h"
#include "GHUtils/GHPropertyContainer.h"
#include "GHGUIProperties.h"
#include "GHPlatform/GHDebugMessage.h"

class GHGUIWidget;

GHGUIPoseableCanvasControllerXMLLoader::GHGUIPoseableCanvasControllerXMLLoader(const GHInputState& inputState)
	: mInputState(inputState)
{

}

void* GHGUIPoseableCanvasControllerXMLLoader::create(const GHXMLNode& node, GHPropertyContainer& extraData) const
{
	unsigned int gamepadIndex = 0;
	node.readAttrUInt("gamepadIndex", gamepadIndex);
	unsigned int poseableIndex = 0;
	node.readAttrUInt("poseableIndex", poseableIndex);

	if (gamepadIndex >= mInputState.getNumGamepads())
	{
		GHDebugMessage::outputString("Error creating GHGUIPoseableCanvasController: invalid gamepad index (there are %u gamepads)", mInputState.getNumGamepads());
		return 0;
	}

	if (poseableIndex >= mInputState.getGamepad(gamepadIndex).mPoseables.size())
	{
		GHDebugMessage::outputString("Error creating GHGUIPoseableCanvasController: invalid poseable index (gamepad %u has %zu poseables)", gamepadIndex, mInputState.getGamepad(gamepadIndex).mPoseables.size());
		return 0;
	}

	GHGUIWidget* widget = extraData.getProperty(GHGUIProperties::GP_PARENTWIDGET);

	if (!widget)
	{
		GHDebugMessage::outputString("Error creating GHGUIPoseableCanvasController: no widget on extraData");
		return 0;
	}

	GHTransform offsetTransform;
	offsetTransform.becomeIdentity();
	node.readAttrFloatArr("offsetTransform", offsetTransform.getMatrix().getArr(), 16);

	bool debugMove = false;
	node.readAttrBool("debugMove", debugMove);

	return new GHGUIPoseableCanvasController(mInputState, *widget, gamepadIndex, poseableIndex, offsetTransform, debugMove);
}
