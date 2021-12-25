#pragma once

#include "Render/GHTexture.h"
#include "GHDX12Include.h"
#include "Render/GHMDesc.h"

class GHRenderDeviceDX12;

class GHTextureDX12 : public GHTexture
{
public:
	GHTextureDX12(GHRenderDeviceDX12& device, Microsoft::WRL::ComPtr<ID3D12Resource> dxBuffer, void* mem, DXGI_FORMAT dxFormat, bool mipmap);
	~GHTextureDX12(void);

	// todo: remove the no argument bind from the interface.
	virtual void bind(void) override { assert(false); }
	void bind(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int heapTextureStart, unsigned int index);
	void createSampler(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> heap, unsigned int index, GHMDesc::WrapMode wrapMode);

	virtual bool lockSurface(void** ret, unsigned int& pitch) override { return false; }
	virtual bool unlockSurface(void) override { return false; }
	virtual bool getDimensions(unsigned int& width, unsigned int& height, unsigned int& depth) override { return false; }

	virtual void deleteSourceData(void) override;

	Microsoft::WRL::ComPtr<ID3D12Resource> getDXBuffer(void) const { return mDXBuffer; }
	DXGI_FORMAT getDXFormat(void) const { return mDXFormat; }

private:
	GHRenderDeviceDX12& mDevice;
	Microsoft::WRL::ComPtr<ID3D12Resource> mDXBuffer;
	void* mMem;
	DXGI_FORMAT mDXFormat;
	bool mMipmap;
};
