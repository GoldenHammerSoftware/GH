#pragma once

#include "Render/GHRenderDevice.h"
#include "GHDX12Include.h"
#include "GHDX12CommandList.h"
#include "GHDX12RTGroup.h"

class GHWin32Window;
class GHDX12Fence;
class GHRenderTarget;

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

	Microsoft::WRL::ComPtr<ID3D12Device2> getDXDevice(void) { return mDXDevice; }
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> getRenderCommandList(void);
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> getCommandQueue(void) { return mDXCommandQueue; }
	Microsoft::WRL::ComPtr<ID3D12RootSignature> getGraphicsRootSignature(void) { return mGraphicsRootSignature; }
	int32_t getFrameBackendId(void) const { return mCurrBackend; }
	const GHDX12RTGroup& getActiveRTGroup(void) const { return mActiveRTGroup; }

	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> beginUploadCommandList(void);
	void endUploadCommandList(void);
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> beginComputeCommandList(void);
	void endComputeCommandList(void);

	// wait for all command buffers to finish so we can delete resources etc.
	void flushGPU(void);

	void applyDefaultTarget(void);
	void applyRenderTarget(const GHDX12RTGroup& rtGroup);
	void applyRenderRootSignature(void);

private:
	void createGraphicsRootSignature(void);
	void createDepthBuffer(void);
	void setClearColor(const GHPoint4& color);
	void clearBuffers(void);

protected:
	GHViewInfo mViewInfo;
	GHWin32Window& mWindow;
	Microsoft::WRL::ComPtr<ID3D12Device2> mDXDevice{ nullptr };
	Microsoft::WRL::ComPtr<IDXGISwapChain1> mDXSwapChain{ nullptr };
	DXGI_SAMPLE_DESC mDXSwapChainSampleDesc;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDXDescriptorHeap{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> mDXCommandQueue{ nullptr };
	Microsoft::WRL::ComPtr<ID3D12RootSignature> mGraphicsRootSignature{ nullptr };

	GHDX12CommandList* mUploadCommandList;
	GHDX12CommandList* mComputeCommandList;

	// The info needed for one frame in our swap buffer.
	struct FrameBackend
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mBackBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> mMsaaBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mBackBufferRTV;
		GHDX12CommandList* mCommandList;
	};
	FrameBackend mFrameBackends[NUM_SWAP_BUFFERS];
	int32_t mCurrBackend{ -1 };

	// only one depth buffer for the whole app?
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDepthDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuffer;
	
	GHDX12RTGroup mActiveRTGroup;

	D3D12_RECT mScissorRect;
	D3D12_VIEWPORT mViewport;
	bool mVSyncEnabled{ false };
	GHPoint4 mClearColor;
	GHRenderTarget* mRenderTarget{ 0 };
};

