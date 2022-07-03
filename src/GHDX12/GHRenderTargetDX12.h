#pragma once

#include "Render/GHRenderTarget.h"
#include "GHDX12Include.h"
#include "GHDX12RenderTargetUtil.h"

class GHRenderDeviceDX12;
class GHTextureDX12;
class GHMipmapGeneratorDX12;
class GHDX12DescriptorHeap;

class GHRenderTargetDX12 : public GHRenderTarget
{
public:
	GHRenderTargetDX12(GHRenderDeviceDX12& device, const GHRenderTarget::Config& args, GHMipmapGeneratorDX12& mipGen);
	~GHRenderTargetDX12(void);

	virtual void apply(void) override;
	virtual void remove(void) override;
	virtual GHTexture* getTexture(void) override;

	void resize(const GHRenderTarget::Config& args);

private:
	void createDXBuffers(void);
	void createDepthBuffers(void);
	void createColorBuffers(void);
	void resolveMsaa(void);

private:
	GHRenderTarget::Config mConfig;
	GHRenderDeviceDX12& mDevice;
	GHMipmapGeneratorDX12& mMipGen;

	GHDX12DescriptorHeap* mColorDescriptorHeap{ 0 };
	GHDX12DescriptorHeap* mDepthDescriptorHeap{ 0 };
	D3D12_VIEWPORT mViewport;

	GHDX12RenderTargetUtil::FrameInfo mFrames[NUM_SWAP_BUFFERS];
};
