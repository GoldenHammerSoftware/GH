#include "GHScreenshotCapturer.h"
#include "GHRenderTarget.h"
#include "GHRenderTargetFactory.h"
#include "GHTexture.h"
#include "GHTextureBMPSaver.h"
#include "GHSceneRenderer.h"
#include "GHRenderDevice.h"
#include "GHPlatform/GHDebugMessage.h"
#include "GHPlatform/GHTimeVal.h"

GHScreenshotCapturer::GHScreenshotCapturer(const GHFileOpener& fileOpener,
										   const GHTimeVal& timeVal,
										   GHRenderDevice& renderDevice,
										   GHSceneRenderer& sceneRenderer,
										   const GHRenderTargetFactory& renderTargetFactory,
										   const GHIdentifier& passID,
										   const GHString& saveFilenameSeed)
	: mFileOpener(fileOpener)
	, mTimeVal(timeVal)
	, mRenderDevice(renderDevice)
	, mSaveFilenameSeed(saveFilenameSeed)
		
{
	//GHRenderTarget::Config rtConfig()
}

GHScreenshotCapturer::~GHScreenshotCapturer(void)
{

}

void GHScreenshotCapturer::activate(void)
{
	GHTexture* texture = mRenderDevice.resolveBackbuffer();
	if (!texture) {
		GHDebugMessage::outputString("Error in GHScreenshotCapturer: resolveBackBuffer");
		return;
	}

	unsigned int width, height, depth;
	texture->getDimensions(width, height, depth);

	//Time val just used to make sure the filenames are unique. we don't care what the value is.
	constexpr int bufSz = 512;
	char filenameBuffer[bufSz];
	snprintf(filenameBuffer, bufSz, "%s_%ux%u_%x.bmp", mSaveFilenameSeed.getChars(), width, height, (unsigned int)mTimeVal.getTime());

	//Making assumptions about our specific texture here. 
	// Potential todo: make it readable from GHTexture?
	// we could then remove it from the GHTextureBMPSaver interface
	GHTextureFormat::Enum srcFormat = GHTextureFormat::TF_BGRA8;
	GHTextureFormat::Enum dstFormat = GHTextureFormat::TF_BGR8;
	//GHTextureFormat::Enum dstFormat = GHTextureFormat::TF_RGB8;

	GHTextureBMPSaver saver(mFileOpener);
	saver.saveTexture(*texture, filenameBuffer, srcFormat, dstFormat);
	GHDebugMessage::outputString("Saved texture %s", filenameBuffer);

	delete texture;
}

void GHScreenshotCapturer::deactivate(void)
{
}
