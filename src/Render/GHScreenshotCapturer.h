#pragma once

#include "GHUtils/GHTransition.h"
#include "Base/GHKeyDef.h"
#include "GHString/GHIdentifier.h"
#include "GHString/GHString.h"

class GHFileOpener;
class GHRenderDevice;
class GHSceneRenderer;
class GHRenderTargetFactory;
class GHRenderTarget;
class GHRenderPass;
class GHTimeVal;

class GHScreenshotCapturer : public GHTransition
{
public:
	GHScreenshotCapturer(const GHFileOpener& fileOpener, 
						 const GHTimeVal& timeVal,
						 GHRenderDevice& renderDevice,
						 GHSceneRenderer& sceneRenderer,
						 const GHRenderTargetFactory& renderTargetFactory,
						 const GHIdentifier& passID,
						 const GHString& saveFilenameSeed);

	virtual ~GHScreenshotCapturer(void);

	virtual void activate(void);
	virtual void deactivate(void);

private:


private:
	GHString mSaveFilenameSeed;
	GHKeyDef::Enum mCaptureHotkey;
	const GHFileOpener& mFileOpener;
	const GHTimeVal& mTimeVal;
	GHRenderDevice& mRenderDevice;
	
	GHRenderTarget* mRenderTarget{ 0 };
	GHRenderPass* mRenderPass{ 0 };
};
