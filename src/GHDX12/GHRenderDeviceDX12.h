#pragma once

#include "Render/GHRenderDevice.h"
#include "GHDX12Include.h"

class GHWin32Window;
class GHDX12Fence;

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

	Microsoft::WRL::ComPtr<ID3D12Device2>& getDXDevice(void) { return mDXDevice; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& getRenderCommandList(void);
	ID3D12RootSignature* getGraphicsRootSignature(void) { return mGraphicsRootSignature; }
	DXGI_SAMPLE_DESC getSampleDesc(void) const { return mDXSwapChainSampleDesc; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList>& beginUploadCommandList(void);
	void endUploadCommandList(void);

private:
	void createGraphicsRootSignature(void);

protected:
	GHViewInfo mViewInfo;
	GHWin32Window& mWindow;
	Microsoft::WRL::ComPtr<ID3D12Device2> mDXDevice;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mDXSwapChain;
	DXGI_SAMPLE_DESC mDXSwapChainSampleDesc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDXDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mDXCommandQueue;

	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDXUploadCommandAllocator;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mDXUploadCommandList;

	ID3D12RootSignature* mGraphicsRootSignature{ nullptr };

	// The info needed for one frame in our swap buffer.
	struct FrameBackend
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffer;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> mDXCommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> mDXCommandList;
		GHDX12Fence* mFence;
		uint64_t mFenceWaitVal;
	};

	FrameBackend mFrameBackends[NUM_SWAP_BUFFERS];
	int32_t mCurrBackend{ -1 };
};

