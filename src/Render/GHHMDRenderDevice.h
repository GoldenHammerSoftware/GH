#pragma once

class GHViewInfo;
class GHCamera;
class GHRenderTarget;
class GHFrustum;

// interface for a head mounted display's influence on a render device.
class GHHMDRenderDevice
{
public:
	enum RenderTargetType
	{
		RTT_BOTH,
		RTT_LEFT,
		RTT_RIGHT
	};

public:
	virtual ~GHHMDRenderDevice(void) {}

	virtual void beginFrame(void) = 0;
	virtual void endFrame(void) = 0;

	virtual void createStereoView(const GHViewInfo& mainView, const GHCamera& camera,
		GHViewInfo& leftView, GHViewInfo& rightView, GHFrustum& combinedFrustum) const = 0;

	// are we activated and being worn, ie should the game render in stereo to us.
	virtual bool isActive(void) const = 0;

	virtual GHRenderTarget* getRenderTarget(RenderTargetType targetType) const = 0;
};
