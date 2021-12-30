#pragma once

#include "Render/GHRenderTarget.h"
#include "GHDX12Include.h"

class GHRenderDeviceDX12;
class GHTextureDX12;
class GHMipmapGeneratorDX12;

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

private:
	GHRenderTarget::Config mConfig;
	GHRenderDeviceDX12& mDevice;
	GHMipmapGeneratorDX12& mMipGen;

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mColorDescriptorHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mDepthDescriptorHeap;
	D3D12_VIEWPORT mViewport;

	struct FrameInfo
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> mColorBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mColorBufferRTV;

		Microsoft::WRL::ComPtr<ID3D12Resource> mDepthBuffer;
		D3D12_CPU_DESCRIPTOR_HANDLE mDepthBufferRTV;

		// optional target for resolve if msaa is true.
		Microsoft::WRL::ComPtr<ID3D12Resource> mResolveBuffer;

		GHTextureDX12* mTexture{ 0 };
	};
	FrameInfo mFrames[NUM_SWAP_BUFFERS];
};
