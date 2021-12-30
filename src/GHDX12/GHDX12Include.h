// Copyright Golden Hammer Software
#ifndef GHDX12INCLUDE_H
#define GHDX12INCLUDE_H

#include <wrl/client.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <DirectXMath.h>

#define NUM_SWAP_BUFFERS 2
#define SWAP_BUFFER_FORMAT DXGI_FORMAT_R8G8B8A8_UNORM
#define DEPTH_BUFFER_FORMAT DXGI_FORMAT_D32_FLOAT
#define DEPTH_BUFFER_FORMAT_SRV DXGI_FORMAT_R32_FLOAT
#define MAX_TEXTURES 16
#define MSAA_SAMPLE_COUNT 4

#endif