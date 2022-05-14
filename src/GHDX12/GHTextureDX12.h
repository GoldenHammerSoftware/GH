#pragma once

#include "Render/GHTexture.h"
#include "GHDX12Include.h"
#include "Render/GHMDesc.h"

class GHRenderDeviceDX12;
struct GHTextureData;
class GHMipmapGeneratorDX12;

class GHTextureDX12 : public GHTexture
{
public:
	GHTextureDX12(GHRenderDeviceDX12& device, GHTextureData* texData, bool mipmap, GHMipmapGeneratorDX12* mipGen);
	GHTextureDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, DXGI_FORMAT dxFormat, bool mipmap);
	~GHTextureDX12(void);

	// todo: remove the no argument bind from the interface.
	virtual void bind(void) override { assert(false); }
	void bind(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int heapTextureStart, unsigned int index);
	void createSampler(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int index, GHMDesc::WrapMode wrapMode);

	virtual bool lockSurface(void** ret, unsigned int& pitch) override;
	virtual bool unlockSurface(void) override;
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth) override;

	virtual void deleteSourceData(void) override;

	Microsoft::WRL::ComPtr<ID3D12Resource> getDXBuffer(void) const { return mDXBuffer; }
	DXGI_FORMAT getDXFormat(void) const { return mDXFormat; }

private:
	void createSrvDesc(void);

private:
	GHRenderDeviceDX12& mDevice;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDXBuffer{ nullptr };
	GHTextureData* mTexData{ 0 };
	bool mMipmap{ false };
	DXGI_FORMAT mDXFormat{ DXGI_FORMAT_UNKNOWN };
	D3D12_SHADER_RESOURCE_VIEW_DESC mSrvDesc;
};
