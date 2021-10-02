#pragma once

#include "Render/GHRenderDevice.h"
#include "GHDX12Include.h"

class GHWin32Window;

class GHRenderDeviceDX12 : public GHRenderDevice
{
public:
	GHRenderDeviceDX12(GHWin32Window& window);
	~GHRenderDeviceDX12(void);

	void reinit(void);

	virtual bool beginFrame(void) override;
	virtual void endFrame(void) override;

	virtual void beginRenderPass(GHRenderTarget* optionalTarget, const GHPoint4& clearColor, bool clearBuffers) override;
	virtual void endRenderPass(void) override;

	virtual void applyViewInfo(const GHViewInfo& viewInfo) override;
	virtual void createDeviceViewTransforms(const GHViewInfo::ViewTransforms& engineTransforms,
		GHViewInfo::ViewTransforms& deviceTransforms,
		const GHCamera& camera, bool isRenderToTexture) const override;
	virtual const GHViewInfo& getViewInfo(void) const override;

	virtual void handleGraphicsQualityChange(void) override;

	virtual GHTexture* resolveBackbuffer(void) override;

protected:
	#define NUM_SWAP_BUFFERS 3
	GHViewInfo mViewInfo;
	GHWin32Window& mWindow;
	Microsoft::WRL::ComPtr<ID3D12Device2> mDXDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mDXSwapChain;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDXDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffers[NUM_SWAP_BUFFERS];

	// Direct commands.  todo: abstract for compute etc.
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mDXCommandQueue;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDXCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mDXCommandList;
};

