#include "GHEnvironmentCapturer.h"
#include "Base/GHInputState.h"
#include "GHSceneRenderer.h"
#include "GHMath/GHTransform.h"
#include "GHRenderTarget.h"
#include "GHRenderTargetFactory.h"
#include "GHTexture.h"
#include "GHTextureBMPSaver.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHMath/GHPi.h"

GHEnvironmentCapturer::GHEnvironmentCapturer(const GHFileOpener& fileOpener,
											 const GHInputState& inputState,
											 const GHCamera& sourceCamera,
											 GHRenderDevice& renderDevice,
											 GHSceneRenderer& sceneRenderer,
											 const GHRenderTargetFactory& renderTargetFactory,
											 const GHIdentifier& passID,
											 int keyboardIndex,
											 GHKeyDef::Enum captureHotkey,
											 int outputResolution)
	: mCaptureHotkey(captureHotkey)
	, mKeyboardIndex(keyboardIndex)
	, mFileOpener(fileOpener)
	, mInputState(inputState)
	, mSourceCamera(sourceCamera)
	, mSceneRenderer(sceneRenderer)
{
	updateCamera();

	GHRenderTarget::Config rtConfig(outputResolution, outputResolution, false, false, "envcapture");
	mRenderTarget = renderTargetFactory.createRenderTarget(rtConfig);
	mScreenInfo.setSize(GHPoint2i(outputResolution, outputResolution));
	GHPoint4 clearColor(0, 0, 0, 1);
	mRenderPass = new GHRenderPass(mCamera, mScreenInfo, mRenderTarget, renderDevice, true, &clearColor, passID, false, true);
}

GHEnvironmentCapturer::~GHEnvironmentCapturer(void)
{
	delete mRenderPass;
	delete mRenderTarget;
}

void GHEnvironmentCapturer::onActivate(void)
{

}

void GHEnvironmentCapturer::onDeactivate(void)
{

}

void GHEnvironmentCapturer::update(void)
{
	if (mInputState.checkKeyChange(mKeyboardIndex, mCaptureHotkey, true))
	{
		GHPoint3 position;
		mSourceCamera.getTransform().getTranslate(position);
		capture(position);
	}

	size_t numPresets = mPresets.size();
	for (size_t i = 0; i < numPresets; ++i)
	{
		auto& preset = mPresets[i];
		if (mInputState.checkKeyChange(mKeyboardIndex, preset.mHotkey, true))
		{
			capture(preset.mPosition);
		}
	}
}

void GHEnvironmentCapturer::updateCamera(void)
{
	mCamera = mSourceCamera; // Inherit any unspecified values from source camera
	mCamera.setFOV(90.f); // FOV always 90 degrees
}

void GHEnvironmentCapturer::capture(const GHPoint3& position)
{
	GHDebugMessage::outputString("Capturing Cubemap: add the following line to your GHEnvironmentCapturer creation to save it and pick a hotkey:");
	GHDebugMessage::outputString("envCapturer->addPresetCaptureSpot(GHEnvironmentCapturer::Preset(GHPoint3(%f, %f, %f), GHKeyDef::Enum('')));", position[0], position[1], position[2]);

	updateCamera();

	mCamera.getTransform().becomeIdentity();
	mCamera.getTransform().setTranslate(position);
	renderToFile("posz.bmp");

	mCamera.getTransform().becomeYRotation(GHPI);
	mCamera.getTransform().setTranslate(position);
	renderToFile("negz.bmp");

	mCamera.getTransform().becomeYRotation(-GHPI / 2.f);
	mCamera.getTransform().setTranslate(position);
	renderToFile("posx.bmp");

	mCamera.getTransform().becomeYRotation(GHPI / 2.f);
	mCamera.getTransform().setTranslate(position);
	renderToFile("negx.bmp");

	mCamera.getTransform().becomeXRotation(-GHPI / 2.f);
	mCamera.getTransform().setTranslate(position);
	renderToFile("posy.bmp");

	mCamera.getTransform().becomeXRotation(GHPI / 2.f);
	mCamera.getTransform().setTranslate(position);
	renderToFile("negy.bmp");
}

void GHEnvironmentCapturer::renderToFile(const char* filename)
{
	mSceneRenderer.drawRenderPass(*mRenderPass);

	GHTexture* texture = mRenderTarget->getTexture();
	if (!texture) {
		GHDebugMessage::outputString("Error in GHEnvironmentCapturer: render pass does not have a texture we can read. Cannot save %s", filename);
		return;
	}

	//Making assumptions about our specific texture here. 
	// Potential todo: make it readable from GHTexture?
	// we could then remove it from the GHTextureBMPSaver interface
	GHTextureFormat::Enum srcFormat = GHTextureFormat::TF_RGBA8;
	GHTextureFormat::Enum dstFormat = GHTextureFormat::TF_RGBA8;
	
	GHTextureBMPSaver saver(mFileOpener);
	saver.saveTexture(*texture, filename, srcFormat, dstFormat);
}

void GHEnvironmentCapturer::addPresetCaptureSpot(const Preset& preset)
{
	mPresets.push_back(preset);
}
