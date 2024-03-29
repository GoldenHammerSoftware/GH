#pragma once

#include "GHDX12Include.h"

struct GHDX12RTGroup
{
	D3D12_CPU_DESCRIPTOR_HANDLE mRt0{ 0 };
	DXGI_FORMAT mRt0Format{ SWAP_BUFFER_FORMAT };
	D3D12_CPU_DESCRIPTOR_HANDLE mDepth{ 0 };
	DXGI_FORMAT mDepthFormat{ DEPTH_BUFFER_FORMAT };
	uint32_t mSampleCount{ 1 };
	uint32_t mSampleQuality{ 0 };
};
