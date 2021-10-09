#pragma once

#include "GHUtils/GHController.h"
#include "Base/GHKeyDef.h"
#include "GHMath/GHPoint.h"
#include <vector>
#include "GHCamera.h"
#include "GHScreenInfo.h"

class GHInputState;
class GHSceneRenderer;
class GHRenderTarget;
class GHRenderTargetFactory;
class GHRenderDevice;
class GHFileOpener;
class GHRenderPass;

class GHEnvironmentCapturer : public GHController
{
public:
	GHEnvironmentCapturer(const GHFileOpener& fileOpener,
						  const GHInputState& inputState, 
						  const GHCamera& sourceCamera,
						  GHRenderDevice& renderDevice,
						  GHSceneRenderer& sceneRenderer,
						  const GHRenderTargetFactory& renderTargetFactory,
						  const GHIdentifier& passID,
						  int keyboardIndex,
						  GHKeyDef::Enum captureHotkey, 
						  int outputResolution);

	virtual ~GHEnvironmentCapturer(void);

	virtual void onActivate(void);
	virtual void onDeactivate(void);
	virtual void update(void);

	struct Preset
	{
		GHPoint3 mPosition;
		GHKeyDef::Enum mHotkey;

		Preset(const GHPoint3& position, GHKeyDef::Enum hotkey)
			: mPosition(position), mHotkey(hotkey)
		{}
	};

	void addPresetCaptureSpot(const Preset& preset);
	GHRenderTarget* getRenderTarget(void) { return mRenderTarget; }

private:
	void capture(const GHPoint3& position);
	void renderToFile(const char* filename);
	void updateCamera(void);

private:
	GHCamera mCamera;
	GHScreenInfo mScreenInfo;
	std::vector<Preset> mPresets;
	GHKeyDef::Enum mCaptureHotkey;
	int mKeyboardIndex;
	const GHFileOpener& mFileOpener;
	const GHCamera& mSourceCamera;
	const GHInputState& mInputState;
	GHSceneRenderer& mSceneRenderer;

	GHRenderTarget* mRenderTarget;
	GHRenderPass* mRenderPass;
};
